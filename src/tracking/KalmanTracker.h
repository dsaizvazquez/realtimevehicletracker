#ifndef KALMAN_F
#define KALMAN_F

#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

//TODO: Unfinished, basic structure in process
class KalmanTracker{
    
    cv::KalmanFilter kf;

    public:
        int init(cv::Rect initialState);

    
};
#endif