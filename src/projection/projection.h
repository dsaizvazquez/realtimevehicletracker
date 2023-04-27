#include <opencv2/opencv.hpp>
#include <spdlog/spdlog.h>


typedef struct ProjectionConfiguration
{
	float aspectRatio=1;
    float offsetX=0;
    float offsetY=0;
    float sensor_width=1;
    float frame_width = 1;
    float frame_height=1;
    float focalLength=0.005;
}ProjectionConfiguration;


namespace projection{
    cv::Point3d projectPointToFlatPlane(cv::Mat K, cv::Mat R, float H, cv::Mat homPos);
    cv::Mat rotationMatrixFromAngles(cv::Vec3f &theta);
}