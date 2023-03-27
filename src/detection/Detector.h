#ifndef DETECTOR
#define DETECTOR

#include <opencv2/opencv.hpp>
#include <fstream>
#include <spdlog/spdlog.h>

const int YOLOV8 = 8;
const int YOLOV5 = 5;
const int YOLOV7 = 7;


typedef struct Detection
{
	float confidence;
	cv::Rect box;
  int class_id;
}Detection;

typedef struct DetectionConfiguration
{
    float InputWidth = 640.0;
    float InputHeight = 640.0;
    float ScoreThreshold = 0.5;
    float NmsThreshold = 0.45;
    float ConfidenceThreshold = 0.45;
    int yolov = 5;

}DetectionConfiguration;


class Detector {

    
    cv::dnn::Net &net;
    const std::vector<std::string> &class_name;

    DetectionConfiguration * config;
    
    std::vector<cv::Mat> outputs;
    std::vector<Detection> detections;
    

    //yolov7 parsing variables
    const int strideSize = 3;   //stride size
    const float netStride[4] = { 8, 16.0,32,64 }; //net stride
    const float netAnchors[3][6] = { {12, 16, 19, 36, 40, 28},{36, 75, 76, 55, 72, 146},{142, 110, 192, 243, 459, 401} }; //net anchors
    float sigmoid_x(float x)
    {
      return static_cast<float>(1.f / (1.f + exp(-x)));
    }

  public:
    int_fast8_t forward(cv::Mat &input_image);
    int_fast8_t process(float x_factor,float y_factor);
    std::vector<Detection> detect(cv::Mat &input_image);
    Detector(cv::dnn::Net &neti, const std::vector<std::string> &class_namei, DetectionConfiguration * config) :net(neti), class_name(class_namei), config(config){};

};

#endif