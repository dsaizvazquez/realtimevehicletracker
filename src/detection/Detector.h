#ifndef DETECTOR
#define DETECTOR

#include <opencv2/opencv.hpp>
#include <fstream>
#include <spdlog/spdlog.h>


#include "constants.h"

typedef struct Detection
{
	float confidence;
	cv::Rect box;
  int class_id;
}Detection;


class Detector {

    
    cv::dnn::Net &net;
    const std::vector<std::string> &class_name;
    
    std::vector<cv::Mat> outputs;
    std::vector<Detection> detections;

  public:
    int_fast8_t forward(cv::Mat &input_image);
    int_fast8_t process(float x_factor,float y_factor);
    std::vector<Detection> detect(cv::Mat &input_image);
    Detector(cv::dnn::Net &neti, const std::vector<std::string> &class_namei) :net(neti), class_name(class_namei){};

};

#endif