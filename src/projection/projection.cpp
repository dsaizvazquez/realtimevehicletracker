#include "projection.h"


cv::Mat R_c;


cv::Point3d projection::projectPointToFlatPlane(cv::Mat K, cv::Mat R, float H, cv::Mat homPos){
        //spdlog::info("{},{}",homPos.at<float>(0,0),homPos.at<float>(1,0));
        cv::Mat R_inv=  R.inv();
        cv::Mat R_c_inv = R_c.inv();
        cv::Mat K_inv =K.inv();
        cv::Mat M = R_inv*R_c_inv*K_inv*homPos;
        //cv::Mat M = homPos*K_inv*R_c_inv*R_inv;
        float s = -(H)/M.at<float>(2,0);
        
        cv::Mat res =M*s;
        cv::Point3d p (res.at<float>(0,0), res.at<float>(1,0), res.at<float>(2,0)+H);
        return p;
};

cv::Mat projection::rotationMatrixFromAngles(cv::Vec3f &theta){

        // Calculate rotation about x axis
        cv::Mat R_x = (cv::Mat_<float>(3,3) <<
                1,       0,              0,
                0,       cos(theta[0]),   sin(theta[0]),
                0,       -sin(theta[0]),   cos(theta[0])
                );
    
        // Calculate rotation about y axis
        cv::Mat R_y = (cv::Mat_<float>(3,3) <<
                cos(theta[1]),    0,     -sin(theta[1]),
                0,               1,      0,
                sin(theta[1]),   0,      cos(theta[1])
                );
    
        // Calculate rotation about z axis
        cv::Mat R_z = (cv::Mat_<float>(3,3) <<
                cos(theta[2]),    sin(theta[2]),      0,
                -sin(theta[2]),   cos(theta[2]),       0,
                0,               0,                  1);
    
        // Combined rotation matrix
        cv::Mat R = R_x * R_y * R_z;
    
        return R;
    
}