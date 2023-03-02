#include <opencv2/opencv.hpp>

namespace projection{
    cv::Point3d projectPointToFlatPlane(cv::Mat K, cv::Mat R, float H, cv::Mat homPos);
    cv::Mat rotationMatrixFromAngles(cv::Vec3f &theta);
}