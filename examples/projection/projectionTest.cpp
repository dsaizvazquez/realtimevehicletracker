#include <opencv2/opencv.hpp>
#include "../../src/projection/projection.h"
#include "yaml-cpp/yaml.h"



using namespace std;

#include <cmath>

double deg2rad(double deg) {
    return deg * M_PI / 180.0;
}

int main()
{
    YAML::Node configYAML = YAML::LoadFile("projection.yaml");
    float pitch=configYAML["pitch"].as<float>();
    float yaw=configYAML["yaw"].as<float>();
    float roll=configYAML["roll"].as<float>();
    float altitude=configYAML["altitude"].as<float>();
    float focalLength=configYAML["focalLength"].as<float>();

    float x=configYAML["x"].as<float>();
    float y=configYAML["y"].as<float>();



    float aspectRatio = 0.75;
    float offsetX = 0;
    float offsetY =0;

    cv::Mat K =(cv::Mat_<float>(3,3) <<
                focalLength,       0,                         offsetX,
                0,                 focalLength*aspectRatio,   offsetY,
                0,                  0,                         -1 );

    cv::Vec3f theta(deg2rad(roll),deg2rad(pitch),deg2rad(yaw));
    cv::Mat R = projection::rotationMatrixFromAngles(theta);
    cv::Vec3f theta2(deg2rad(90),deg2rad(0),deg2rad(-90));
    R_c= projection::rotationMatrixFromAngles(theta2);
    // cv::Mat rotPos = (cv::Mat_<float>(3,1) << 0, 1, 0);
    // std::cout<<R*rotPos<<std::endl;

    std::cout<<K<<"\r"<<R<<std::endl;
    cv::Mat position = (cv::Mat_<float>(3,1) << x, y, 1);
    cv::Point3f res = projection::projectPointToFlatPlane(K, R, altitude, position);
    std::cout<<position<<res<<std::endl;

    return 0;
}