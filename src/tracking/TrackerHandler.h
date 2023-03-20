#include "KalmanTracker.h"
#include "../detection/Detector.h"
#include "hungarian/Hungarian.h"
#include "udpThread/udpConn.hpp"


class TrackerHandler{

    double iouThreshold = 0.3;
    int maxAge = 3;
    int idCounter=0;
    
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

    UDPConn connection;
    ProjectionParams params;

    float aspectRatio=1;
    float offsetX=0;
    float offsetY=0;
    float sensor_width=1;
    float frame_width = 1;

    

    double GetIOU(cv::Rect bb_test, cv::Rect bb_gt);

    public:
        TrackerHandler(double iouT, int age,std::string IPv4, std::uint16_t port,float focalLengthp, float aspectRatio, float offsetX, float offsetY, float sensor_width);
        void init(std::vector<Detection> detections);
        void predict();
        void match();
        std::vector<Target> correct(); //DELETE RETURN

        void updateParams(SharedData data); 

        void setFrameWidth(float width);
        
        void projectToPlane();
        void estimateSpeed(float deltaTime); 

        
        std::vector<Target> getTargets(); 

};