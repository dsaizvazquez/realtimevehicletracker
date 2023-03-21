#include <opencv2/opencv.hpp>
#include "../../src/projection/projection.h"

using namespace std;

int main()
{
    float focalLength = 4;
    float aspectRatio = 1;
    float offsetX = 0;
    float offsetY =0;
    cv::Mat K =(cv::Mat_<float>(3,3) <<
                focalLength,       0,                         offsetX,
                0,                 focalLength*aspectRatio,   offsetY,
                0,                  0,                         1 );
    cv::Vec3f theta(0,M_PI/2,0);
    cv::Mat R = projection::rotationMatrixFromAngles(theta);
    
    // cv::Mat rotPos = (cv::Mat_<float>(3,1) << 0, 1, 0);
    // std::cout<<R*rotPos<<std::endl;

    std::cout<<K<<"\r"<<R<<std::endl;
    cv::Point2f pos(3.2,0); //initial position in image
    cv::Mat position = (cv::Mat_<float>(3,1) << pos.x, pos.y, 1);
    cv::Point3f res = projection::projectPointToFlatPlane(K, R, 4, position);
    std::cout<<position<<res<<std::endl;

    return 0;
}