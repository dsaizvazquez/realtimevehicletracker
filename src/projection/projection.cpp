#include "projection.h"

cv::Point3d projection::projectPointToFlatPlane(cv::Mat K, cv::Mat R, float H, cv::Mat homPos){
        cv::Mat M =  R.inv()*K.inv()*homPos;
        float s = H/M.at<double>(0,3);

        cv::Mat res =M*s;
        cv::Point3d p (res.at<double>(0,0), res.at<double>(1,0), res.at<double>(2,0)-H);
        return p;
};

cv::Mat projection::rotationMatrixFromAngles(cv::Vec3f &theta){

        // Calculate rotation about x axis
        cv::Mat R_x = (cv::Mat_<float>(3,3) <<
                1,       0,              0,
                0,       cos(theta[0]),   -sin(theta[0]),
                0,       sin(theta[0]),   cos(theta[0])
                );
    
        // Calculate rotation about y axis
        cv::Mat R_y = (cv::Mat_<float>(3,3) <<
                cos(theta[1]),    0,      sin(theta[1]),
                0,               1,      0,
                -sin(theta[1]),   0,      cos(theta[1])
                );
    
        // Calculate rotation about z axis
        cv::Mat R_z = (cv::Mat_<float>(3,3) <<
                cos(theta[2]),    -sin(theta[2]),      0,
                sin(theta[2]),    cos(theta[2]),       0,
                0,               0,                  1);
    
        // Combined rotation matrix
        cv::Mat R = R_z * R_y * R_x;
    
        return R;
    
}