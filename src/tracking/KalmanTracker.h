#ifndef KALMAN_F
#define KALMAN_F

#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>



//TODO: Unfinished, basic structure in process
class KalmanTracker{
    
    cv::KalmanFilter kf;
    cv::Mat measurement;



    cv::Rect get_rect_xysr(float cx, float cy, float s, float r);


    public:
        int init(cv::Rect initialState,int id);
        cv::Rect predict();
        int correct(cv::Rect state);
        cv::Rect getState();
        cv::Point2d getSpeed();
        int age = 0;
        int id = 0;




    
};

#endif