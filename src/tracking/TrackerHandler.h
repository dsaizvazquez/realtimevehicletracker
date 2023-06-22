#include "KalmanTracker.h"
#include "../detection/Detector.h"
#include "hungarian/Hungarian.h"
#include "udpThread/udpConn.hpp"
#include "../sql/SqlConn.h"

typedef struct TrackerConfiguration
{
	double iouThreshold = 0.3;
    int maxAge = 3;
    float speedAvgFactor=0.7;
    
}TrackerConfiguration;


class TrackerHandler{

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


    std::vector<TargetDetection> targets;
    std::vector<Detection> detections;

    TrackerConfiguration *config;
    UdpConnConfiguration *udpConfig;
    ProjectionConfiguration *projectionConfig;

    UDPConn connection;

    SqlConn *conn;
    TargetDatabase baseTarget;

    cv::Mat K;
    cv::Mat R;
    float H=10;

    

    double GetIOU(cv::Rect bb_test, cv::Rect bb_gt);

    public:
        TrackerHandler(double iouT, int age,std::string IPv4, std::uint16_t port,float focalLengthp, float aspectRatio, float offsetX, float offsetY, float sensor_width, float speedAvgFactor);
        TrackerHandler(TrackerConfiguration *config, ProjectionConfiguration *projectionConfig, UdpConnConfiguration *udpConfig, SqlConn * conn, TargetDatabase target);

        void init(std::vector<Detection> detections);
        
        void predict();
        void match();
        std::vector<TargetDetection> correct(); //DELETE RETURN

        void updateParams(SharedData data); 

        void setProjectionParams(float width, float height);
        
        void projectToPlane();
        void estimateSpeed(float deltaTime); 

        
        std::vector<TargetDetection> getTargets(); 

};