#include "KalmanTracker.h"
#include "../detection/Detector.h"
#include "hungarian/Hungarian.h"

typedef struct Target
{
	float confidence;
    cv::Point speed; 
	cv::Rect box;
    int class_id;
    int id;
}Target;

class TrackerHandler{

    double iouThreshold = 0.3;
    int max_age = 3;
    int id_counter=0;
    
    std::vector<KalmanTracker> trackers;
    std::vector<cv::Rect> predictedBoxes;

	std::vector<std::vector<double>> iouMatrix;
    std::vector<int> assignment;
	std::set<int> unmatchedDetections;
	std::set<int> unmatchedTrajectories;
	std::set<int> allItems;
	std::set<int> matchedItems;
	std::vector<cv::Point> matchedPairs;
    std::vector<Target> targets;
    std::vector<Detection> detections;

    double GetIOU(cv::Rect bb_test, cv::Rect bb_gt);

    public:
        TrackerHandler(double iouT, int age):iouThreshold(iouT),max_age(age){};
        void init(std::vector<Detection> detections);
        void predict();
        void match();
        std::vector<Target> correct();


};