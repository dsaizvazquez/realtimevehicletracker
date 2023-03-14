#include <opencv2/opencv.hpp>
#include <fstream>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/sinks/daily_file_sink.h"
#include <unistd.h>

#include "tcpserver.hpp"
#include <json/json.h>

#include "detection/Detector.h"
#include "tracking/TrackerHandler.h"
#include "drawing.h"
#include "yaml-cpp/yaml.h"

typedef int status;
namespace STATUS{
    status INITIALIZING = 0;
    status ERROR_INPUT = 1;
    status NORMAL = 2;
}

const int colorNum = 20;
cv::Scalar randColor[colorNum];

std::string createJSON(int status, std::string input, std::vector<Target> targets,std::vector<std::string> class_list,Json::StreamWriterBuilder wbuilder){
    Json::Value root; 
    int color[3];

    root["status"] = status;
    root["input"] = input;

    Json::Value array;
    for(Target target : targets)
    {
        Json::Value targetVal;
        targetVal["confidence"] = target.confidence ;
        targetVal["class_id"] = class_list[target.class_id];
        targetVal["id"] = target.id;
        cv::Scalar color =randColor[target.id % colorNum];
        Json::Value colorArray;
        for(int i=0;i<3;i++) colorArray.append(color[i]);
        targetVal["color"] = colorArray;
        targetVal["speed"] = target.speed.x;
        array.append(targetVal);
    }
    root["targets"] = array; 
    return Json::writeString(wbuilder, root);
    
}

void sendClients(std::vector<TCPSocket *> clientList, std::string msg){
    for(int i=0;i<clientList.size();i++){
                    if(clientList[i]->remotePort()==0){
                        clientList.erase(clientList.begin()+i);
                    }
                    clientList[i]->Send(msg);
    }
}

int main(int argc, char * argv[]){

    status status=STATUS::INITIALIZING;
    std::string configFile = "config.yaml";
    int c;
    while ((c = getopt (argc, argv, "hf:")) != -1){
        switch(c) // note the colon (:) to indicate that 'b' has a parameter and is not a switch
        {
        case 'f':
            configFile=optarg;
            break;

        case 'h':
            printf( "[-h] [-f config.yaml] -- program to obtain predictions from image\n where:\n  -h  show this help text\n  -f configuration file\n");
            exit(0);
            break;
        }
    }

    // Load configuration
    YAML::Node config = YAML::LoadFile(configFile);


    //configure logger
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
    console_sink->set_pattern("[%H:%M:%S %z] [%^---%L---%$] %v");

    auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_mt>("logs/log.txt", 2, 30);
    file_sink->set_level(spdlog::level::trace);

    spdlog::logger logger("multi_sink", {console_sink, file_sink});

    spdlog::set_default_logger(std::make_shared<spdlog::logger>(logger));
    spdlog::flush_every(std::chrono::seconds(3));
    spdlog::info("Configuration loaded");

    //create JSON writer
    Json::StreamWriterBuilder wbuilder;
    wbuilder["precision"]=2;
    wbuilder["commentStyle"] = "None";
    wbuilder["indentation"] = ""; //The JSON document is written in a single line


    // Load class list.
    std::vector<std::string> class_list;
    std::ifstream ifs(config["cocoFile"].as<std::string>());
    std::string line;
    while (std::getline(ifs, line))
    {
        class_list.push_back(line);
    }

    // Load model.
    cv::dnn::Net net;
    net = cv::dnn::readNet(config["nnetFile"].as<std::string>());
    net.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
	net.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);
    spdlog::info("NN loaded");

    //create TCP-------------------------------------------------------------------------------------------------
    // Initialize server socket..
    TCPServer tcpServer;
    std::vector<TCPSocket *> clientList;
    // When a new client connected:
    tcpServer.onNewConnection = [&](TCPSocket *newClient) {
        spdlog::info("new client with {} address and {} port has started",newClient->remoteAddress(),newClient->remotePort());
        clientList.push_back(newClient);
        newClient->onMessageReceived = [newClient](string message) {
            spdlog::info("{} sends {}",newClient->remoteAddress(),message);
            newClient->Send("OK!");
        };
        
        newClient->onSocketClosed = [newClient](int errorCode) {
            spdlog::info("Socket closed: {}:{} with error {}",newClient->remoteAddress(),newClient->remotePort(),errorCode);
        };
    };

    // Bind the server to a port.
    tcpServer.Bind(config["tcpPort"].as<int>(), [](int errorCode, string errorMessage) {
        // BINDING FAILED:
        spdlog::info("error binding: {}{}",errorCode, errorMessage);
    });

    // Start Listening the server.
    tcpServer.Listen([](int errorCode, string errorMessage) {
        spdlog::info("error starting listening: {}{}",errorCode, errorMessage);
    });

    //started update counter
    int updateCount=0;
    int updateRate = config["updateRate"].as<int>();


    // Create detector
    Detector detector(net,class_list,config["YOLOv"].as<int>() );
    detector.INPUT_HEIGHT=config["INPUT_HEIGHT"].as<float>();
    detector.INPUT_WIDTH=config["INPUT_WIDTH"].as<float>();


    //Create tracker
    TrackerHandler tracker(config["iouThreshold"].as<double>(),config["maxAge"].as<int>(),config["IP"].as<std::string>(),config["port"].as<int>(),config["focalLength"].as<float>(),config["aspectRatio"].as<float>(),config["offsetX"].as<float>(),config["offsetY"].as<float>());
    std::vector<Target> targets;

    int skippedFrames =config["skippedFrames"].as<int>();
    
    spdlog::info("Tracker and Detector initialized");
    float delay = config["errorDelay"].as<float>();

    //display colors create
    cv::RNG rng(0xFFFFFFFF);

    for (int i = 0; i < colorNum; i++) rng.fill(randColor[i], cv::RNG::UNIFORM, 0, 256);

    // Load video.
    cv::VideoCapture cap;
    cv::VideoWriter video;
    std::string input =config["input"].as<std::string>();

    while(true){
        cap.open(input);
        if(!cap.isOpened()){
            status = STATUS::ERROR_INPUT;
            spdlog::warn("Input is not ready, trying again in {} secs",delay);
            sendClients(clientList, "{\"status\":"+std::to_string(status)+"}");
            usleep(delay*1000000);
            continue;
        }else{
            status = STATUS::NORMAL;
            spdlog::info("Input {} is ready",input);
            cv::Mat frame;
            int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
            int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
            double fps = cap.get(cv::CAP_PROP_FPS)/skippedFrames;
            spdlog::info(fps);
            

            if(config["outputType"].as<char>()=='v'){
                spdlog::info("is a video");
                video.open(config["output"].as<std::string>(), cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(frame_width,frame_height));
            }else{
                video.open(config["output"].as<std::string>(),cv::CAP_GSTREAMER, 0,config["fps"].as<int>(), cv::Size (frame_width, frame_height), true);
            }
            double oldVal =0;
            cap >> frame;
            int64 time=0;
            while(!(frame.empty())){

                //Step 1: detect the bounding boxes
                std::vector<Detection>detections = detector.detect(frame);
                spdlog::debug("boxes detected");


                //Step 2: tracking
                tracker.init(detections);
                spdlog::debug("initialized");

                tracker.predict();
                spdlog::debug("predicted");

                tracker.match();
                spdlog::debug("matched");

                targets = tracker.correct();
                spdlog::debug("corrected");

                

                //Step 3: Project Speed
                tracker.projectToPlane();
                double frameTime =cap.get(cv::CAP_PROP_POS_MSEC)-oldVal;
                tracker.estimateSpeed(frameTime);
                targets = tracker.getTargets();

                //Step 4: Drawing
                for(int i=0;i<targets.size();i++){
                    
                    cv::Rect box = targets[i].box;
                    int left = box.x;
                    int top = box.y;
                    int width = box.width;
                    int height = box.height;
                    cv::Scalar color = randColor[targets[i].id % colorNum];
                    
                    cv::rectangle(frame, cv::Point(left, top), cv::Point(left + width, top + height), color, 3*THICKNESS);
                    //spdlog::info("speeds: {} {} px/frame {} {} px/s",targets[i].speed.x, targets[i].speed.y,targets[i].speed.x*fps,targets[i].speed.y*fps);
                    std::string label = cv::format("%d,%.2f",targets[i].id, targets[i].confidence);
                    std::string labelBottom = cv::format("%.2f,%.2f",targets[i].speed.x*fps,targets[i].speed.y*fps);
                    label = class_list[targets[i].class_id] + ":" + label;
                    // Draw class labels.
                    draw::draw_label(frame, label, left, top,color);
                    draw::draw_label(frame, labelBottom, left, top+height,color);
                }
                
                oldVal = cap.get(cv::CAP_PROP_POS_MSEC);
                double del =(double)cv::getTickCount()-time;
                time = cv::getTickCount();
                std::string label = cv::format("Elapsed time : %f ms", 1000*del/cv::getTickFrequency());
                std::string label2 = cv::format("Frame time : %f ms", frameTime);

                putText(frame, label, cv::Point(20, 40), FONT_FACE, FONT_SCALE, RED);
                putText(frame, label2, cv::Point(20, 60), FONT_FACE, FONT_SCALE, RED);
                spdlog::debug("writing frame");



                video.write(frame);
                cap >> frame;
                if(updateCount>updateRate){
                    std::string pkg = createJSON(status, input, targets,class_list,wbuilder);
                    sendClients(clientList, pkg);
                    updateCount=0;
                }
                updateCount++;
                
                for(int i = 0; i< skippedFrames; i++)  cap >> frame;
            }
        }
    
        
    }
    
    cap.release();
    video.release();

    spdlog::info("end");

    return 0;
}