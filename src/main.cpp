#include <opencv2/opencv.hpp>
#include <fstream>

#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include <spdlog/spdlog.h>
#include "detection/Detector.h"
#include "tracking/TrackerHandler.h"
#include "drawing.h"
#include "yaml-cpp/yaml.h"







int main(int argc, char * argv[]){

    spdlog::info("starting");
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

    // Create detector
    Detector detector(net,class_list);

    //Create tracker
    TrackerHandler tracker(config["iouThreshold"].as<double>(),config["maxAge"].as<int>());
    std::vector<Target> targets;

    // Load video.
    cv::VideoCapture cap(config["input"].as<std::string>());
    cv::Mat frame;
    int frame_width = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    int frame_height = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    cv::VideoWriter video;

    if(config["outputType"].as<char>()=='v'){
        spdlog::info("is a video");
        video.open(config["output"].as<std::string>(), cv::VideoWriter::fourcc('M','J','P','G'), 31, cv::Size(frame_width,frame_height));
    }else{
        video.open(config["output"].as<std::string>(),cv::CAP_GSTREAMER, 0, 16, cv::Size (frame_width, frame_height), true);
    }
    
    cap >> frame;

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

        //Step 4: Drawing
        for(int i=0;i<targets.size();i++){
            cv::Rect box = targets[i].box;
            int left = box.x;
            int top = box.y;
            int width = box.width;
            int height = box.height;
            // Draw bounding box.
            cv::rectangle(frame, cv::Point(left, top), cv::Point(left + width, top + height), BLUE, 3*THICKNESS);
            // Get the label for the class name and its confidence.
            std::string label = cv::format("%d,%.2f,%.2d,%.2d",targets[i].id, targets[i].confidence, targets[i].speed.x,targets[i].speed.y);
            label = class_list[targets[i].class_id] + ":" + label;
            // Draw class labels.
            draw::draw_label(frame, label, left, top);
        }
        spdlog::debug("writing frame");

        video.write(frame);
        cap >> frame;
    }
    cap.release();
    video.release();
    spdlog::info("end");

    return 0;
}