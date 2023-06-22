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

typedef struct Configuration
{
    DetectionConfiguration detection;
    TrackerConfiguration tracker;
    UdpConnConfiguration udpConn;
    ProjectionConfiguration projection;

    std::string cocoFile ="models/coco.names";
    std::string nnetFile="models/yolov5s.onnx";
    int tcpPort =12520;
    int updateRate=5;
    int skippedFrames = 1;
    float errorDelay=5;

    std::string input="rtsp://localhost:8554/in";

    std::string output="appsrc ! videoconvert ! x264enc speed-preset=ultrafast bitrate=1800  key-int-max=40 ! rtspclientsink location=rtspt://localhost:8554/out";
    char outputType ='s';
    int fps =15;

    TargetDatabase defaultTarget {
        5.0,
        0.0,
        0.0,
        0.0,
        TOP, 
        {}};

    std::string sqlHostName = "tcp://127.0.0.1:3306";
    std::string sqlUserName = "root";
    std::string sqlPassword = "XXXXXXXX";
    std::string sqlSchema = "RTVT";

}Configuration;


void setConfig(Configuration *config, YAML::Node configYAML){

    spdlog::info("======================CONFIGURATION======================");
    //basic configurations
    spdlog::info("Basic Configuration**************************************");

    if(configYAML["input"]){
        config->input=configYAML["input"].as<std::string>();  
    }
    spdlog::info("input: {}", config->input);

    if(configYAML["output"]){
        config->output=configYAML["output"].as<std::string>();
    }
    spdlog::info("output: {}", config->output);

    if(configYAML["outputType"]){
        config->outputType=configYAML["outputType"].as<char>();
    }
    spdlog::info("outputType: {}", config->outputType);

    if(configYAML["fps"]){
        config->fps=configYAML["fps"].as<int>();
    }
    spdlog::info("fps: {}", config->fps);

    if(configYAML["cocoFile"]){
        config->cocoFile=configYAML["cocoFile"].as<std::string>();
    }
    spdlog::info("cocoFile: {}", config->cocoFile);

    if(configYAML["nnetFile"]){
        config->nnetFile=configYAML["nnetFile"].as<std::string>();
        
    }
    spdlog::info("nnetFile: {}", config->nnetFile);
    
    if(configYAML["tcpPort"]){
        config->tcpPort=configYAML["tcpPort"].as<int>();
        
    }
    spdlog::info("tcpPort: {}", config->tcpPort);
    
    
    spdlog::info("Secondary Configuration**********************************");

    if(configYAML["updateRate"]){
        config->updateRate=configYAML["updateRate"].as<int>();
        
    }
    spdlog::info("updateRate: {}", config->updateRate);

    if(configYAML["skippedFrames"]){
        config->skippedFrames=configYAML["skippedFrames"].as<int>();
    }
    spdlog::info("skippedFrames: {}", config->skippedFrames);


    if(configYAML["errorDelay"]){
        config->errorDelay=configYAML["errorDelay"].as<int>();
        
    }
    spdlog::info("errorDelay: {}", config->errorDelay);

    
    spdlog::info("Tracker Handler Configuration****************************");

    if(configYAML["iouThreshold"]){
        config->tracker.iouThreshold=configYAML["iouThreshold"].as<double>();
        
    }
    spdlog::info("iouThreshold: {}", config->tracker.iouThreshold);

     if(configYAML["iouThreshold"]){
        config->tracker.iouThreshold=configYAML["iouThreshold"].as<double>();
        
    }
    spdlog::info("iouThreshold: {}", config->tracker.iouThreshold);

    if(configYAML["maxAge"]){
        config->tracker.maxAge=configYAML["maxAge"].as<int>();
        
    }
    spdlog::info("maxAge: {}", config->tracker.maxAge);

    if(configYAML["speedAverageFactor"]){
        config->tracker.speedAvgFactor=configYAML["speedAverageFactor"].as<float>();
        
    }
    spdlog::info("speedAverageFactor: {}", config->tracker.speedAvgFactor);


    spdlog::info("UDP Conn Configuration***********************************");
    if(configYAML["IP"]){
        config->udpConn.IPv4=configYAML["IP"].as<std::string>();
        
    }
    spdlog::info("IPv4: {}", config->udpConn.IPv4);

    if(configYAML["port"]){
        config->udpConn.port=configYAML["port"].as<int>();
        
    }
    spdlog::info("port: {}", config->udpConn.port);

    spdlog::info("Projection Configuration*********************************");

    if(configYAML["focalLength"]){
        config->projection.focalLength=configYAML["focalLength"].as<float>();
        config->udpConn.focalLength=configYAML["focalLength"].as<float>();

        
    }
    spdlog::info("focalLength: {}", config->projection.focalLength);

    if(configYAML["aspectRatio"]){
        config->projection.aspectRatio=configYAML["aspectRatio"].as<float>();

        
    }
    spdlog::info("aspectRatio: {}", config->projection.aspectRatio);

    if(configYAML["offsetX"]){
        config->projection.offsetX=configYAML["offsetX"].as<float>();
        
    }
    spdlog::info("offsetX: {}", config->projection.offsetX);

    if(configYAML["offsetY"]){
        config->projection.offsetY=configYAML["offsetY"].as<float>();
        
    }
    spdlog::info("sensorWidth: {}", config->projection.sensor_width);

    if(configYAML["sensorWidth"]){
        config->projection.sensor_width=configYAML["sensorWidth"].as<float>();
        
    }
    spdlog::info("focalLength: {}", config->projection.focalLength);

    spdlog::info("Detection Configuration**********************************");

    if(configYAML["InputWidth"]){
        config->detection.InputWidth=configYAML["InputWidth"].as<float>();
        
    }
    spdlog::info("InputWidth: {}", config->detection.InputWidth);


    if(configYAML["InputHeight"]){
        config->detection.InputHeight=configYAML["InputHeight"].as<float>();
        
    }
    spdlog::info("InputHeight: {}", config->detection.InputHeight);

    if(configYAML["ScoreThreshold"]){
        config->detection.ScoreThreshold=configYAML["ScoreThreshold"].as<float>();
        
    }
    spdlog::info("ScoreThreshold: {}", config->detection.ScoreThreshold);

    if(configYAML["NmsThreshold"]){
        config->detection.NmsThreshold=configYAML["NmsThreshold"].as<float>();
        
    }
    spdlog::info("NmsThreshold: {}", config->detection.NmsThreshold);

    if(configYAML["ConfidenceThreshold"]){
        config->detection.ConfidenceThreshold=configYAML["ConfidenceThreshold"].as<float>();
        
    }
    spdlog::info("ConfidenceThreshold: {}", config->detection.ConfidenceThreshold);

    if(configYAML["YOLOv"]){
        config->detection.yolov=configYAML["YOLOv"].as<int>();
        
    }
    spdlog::info("yolov: {}", config->detection.yolov);


    spdlog::info("SQL Configuration**********************************");


     if(configYAML["sqlHostName"]){
        config->sqlHostName=configYAML["sqlHostName"].as<std::string>();  
    }
    spdlog::info("sqlHostName: {}", config->sqlHostName);

    if(configYAML["sqlUserName"]){
        config->sqlUserName=configYAML["sqlUserName"].as<std::string>();  
    }
    spdlog::info("sqlUserName: {}", config->sqlUserName);

    if(configYAML["sqlPassword"]){
        config->sqlPassword=configYAML["sqlPassword"].as<std::string>();  
    }
    spdlog::info("sqlPassword: {}", config->sqlPassword);

    if(configYAML["sqlSchema"]){
        config->sqlSchema=configYAML["sqlSchema"].as<std::string>();  
    }
    spdlog::info("sqlSchema: {}", config->sqlSchema);

    
    spdlog::info("TESTING Configuration**********************************");

    if(configYAML["height"]){
        config->defaultTarget.height=configYAML["height"].as<float>();
        config->udpConn.altitude=configYAML["height"].as<float>();
        
    }
    spdlog::info("height: {}", config->defaultTarget.height);

    if(configYAML["roll"]){
        config->defaultTarget.roll=configYAML["roll"].as<float>();
        
    }
    spdlog::info("roll: {}", config->defaultTarget.roll);

    if(configYAML["pitch"]){
        config->defaultTarget.pitch=configYAML["pitch"].as<float>();
        config->udpConn.pitch=configYAML["pitch"].as<float>();

    }
    spdlog::info("pitch: {}", config->defaultTarget.pitch);

    if(configYAML["yaw"]){
        config->defaultTarget.yaw=configYAML["yaw"].as<float>();
        config->udpConn.yaw=configYAML["yaw"].as<float>();

    }
    spdlog::info("yaw: {}", config->defaultTarget.yaw);

    if(configYAML["carLane"]){
        config->defaultTarget.carLane=configYAML["carLane"].as<int>();
        
    }
    spdlog::info("carLane: {}", config->defaultTarget.carLane);






}


const int colorNum = 20;
cv::Scalar randColor[colorNum];

std::string createJSON(int status, std::string input, std::vector<TargetDetection> targets,std::vector<std::string> class_list,Json::StreamWriterBuilder wbuilder){
    Json::Value root; 
    int color[3];

    root["status"] = status;
    root["input"] = input;

    Json::Value array;
    for(TargetDetection target : targets)
    {
        Json::Value targetVal;
        targetVal["confidence"] = target.confidence ;
        targetVal["class_id"] = class_list[target.class_id];
        targetVal["id"] = target.id;
        cv::Scalar color =randColor[target.id % colorNum];
        Json::Value colorArray;
        for(int i=0;i<3;i++) colorArray.append(color[i]);
        targetVal["color"] = colorArray;
        targetVal["speed"] = target.speed;
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
    YAML::Node configYAML = YAML::LoadFile(configFile);
    Configuration config;
    
    setConfig(&config,configYAML);


    //configure logger
    auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
    console_sink->set_level(spdlog::level::info);
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
    std::ifstream ifs(config.cocoFile);
    std::string line;
    while (std::getline(ifs, line))
    {
        class_list.push_back(line);
    }

    // Load model.
    cv::dnn::Net net;
    net = cv::dnn::readNet(config.nnetFile);
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
    tcpServer.Bind(config.tcpPort, [](int errorCode, string errorMessage) {
        // BINDING FAILED:
        spdlog::info("error binding: {}{}",errorCode, errorMessage);
    });

    // Start Listening the server.
    tcpServer.Listen([](int errorCode, string errorMessage) {
        spdlog::info("error starting listening: {}{}",errorCode, errorMessage);
    });

    //started update counter
    int updateCount=0;


    // Create detector
    Detector detector(net,class_list,&config.detection);

    spdlog::info("Connecting to the sql server");
    //create sqlconn
    SqlConn conn(config.sqlHostName, config.sqlUserName, config.sqlPassword,config.sqlSchema);
    spdlog::info("sql connection stablished");


    //Create tracker
    TrackerHandler tracker(&config.tracker,&config.projection, &config.udpConn, &conn,config.defaultTarget);
    std::vector<TargetDetection> targets;

    
    spdlog::info("Tracker and Detector initialized");

    //display colors create
    cv::RNG rng(0xFFFFFFFF);

    for (int i = 0; i < colorNum; i++) rng.fill(randColor[i], cv::RNG::UNIFORM, 0, 256);

    // Load video.
    cv::VideoCapture cap;
    cv::VideoWriter video;

    while(true){
        cap.open(config.input);
        if(!cap.isOpened()){
            status = STATUS::ERROR_INPUT;
            spdlog::warn("Input is not ready, trying again in {} secs",config.errorDelay);
            sendClients(clientList, "{\"status\":"+std::to_string(status)+"}");
            usleep(config.errorDelay*1000000);
            continue;
        }else{
            status = STATUS::NORMAL;
            cv::Mat frame;
            int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
            int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
            tracker.setProjectionParams(frame_width, frame_height);
            double fps = cap.get(cv::CAP_PROP_FPS)/config.skippedFrames;
            spdlog::info("Input {} is ready with framerate {}",config.input,fps);

            

            if(config.outputType=='v'){
                spdlog::info("is a video");
                video.open(config.output, cv::VideoWriter::fourcc('M','J','P','G'), 30, cv::Size(frame_width,frame_height));
            }else{
                video.open(config.output,cv::CAP_GSTREAMER, 0,config.fps, cv::Size (frame_width, frame_height), true);
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
                tracker.estimateSpeed(frameTime/1000);
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
                    std::string label = cv::format("%d,%.2f",targets[i].id, targets[i].confidence);
                    std::string labelBottom = cv::format("%.2f",targets[i].speed);
                    label = class_list[targets[i].class_id] + ":" + label;
                    // Draw class labels.
                    draw::draw_label(frame, label, left, top,color);
                    //draw::draw_label(frame, labelBottom, left, top+height,color);
                    //spdlog::info("pos: {} {} {}",targets[i].speed, targets[i].box.x,targets[i].box.y );
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
                if(updateCount>config.updateRate){
                    std::string pkg = createJSON(status, config.input, targets,class_list,wbuilder);
                    sendClients(clientList, pkg);
                    updateCount=0;
                }
                updateCount++;
                
                for(int i = 0; i< config.skippedFrames; i++)  cap >> frame;
            }
        }
    
        
    }
    
    cap.release();
    video.release();

    spdlog::info("end");

    return 0;
}