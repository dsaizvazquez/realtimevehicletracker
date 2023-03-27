#ifndef KALMAN_F
#define KALMAN_F

#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>

#include "../projection/projection.h"


typedef struct Target
{
	float confidence;
    float speed; 
	cv::Rect box;
    int class_id;
    int id;
}Target;




//TODO: Unfinished, basic structure in process
class KalmanTracker{
    
    cv::KalmanFilter kf;
    cv::Mat measurement;
    
    cv::Point2d posPx;
    cv::Point2d speedPx;
    cv::Rect box;

    std::vector<cv::Point3d> posVector;
    std::vector<cv::Point3d> speedVector;
    float speedAveragingFactor =0.7;

    Target res;

    cv::Rect get_rect_xysr(float cx, float cy, float s, float r);


    public:
        int init(cv::Rect initialState,int id, float speedAveragingFactorp);
        cv::Rect predict();
        int correct(cv::Rect state);

        cv::Rect getBox();
        cv::Point2d getSpeedInPx();
        cv::Point2d getPosInPx();


        void project(cv::Mat K, cv::Mat R, float H,float frame_width, float frame_height); 
        void estimateSpeed(float deltaTime);

        Target getTarget();

        int age = 0;
        int id = 0;
        int class_id = 0;
        float confidence = 0;


};





#endif