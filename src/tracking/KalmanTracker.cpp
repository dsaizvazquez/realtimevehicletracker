
#include "KalmanTracker.h"



int KalmanTracker::init(cv::Rect initalState)
{
	int stateNum = 7;
	int measureNum = 4;
	kf = cv::KalmanFilter(stateNum, measureNum, 0);

	kf.transitionMatrix = (cv::Mat_<float>(stateNum, stateNum) <<
		1, 0, 0, 0, 1, 0, 0,
		0, 1, 0, 0, 0, 1, 0,
		0, 0, 1, 0, 0, 0, 1,
		0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 1);

	setIdentity(kf.measurementMatrix);
	setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-2)); //TODO make it a controllable input parameter
	setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1)); //TODO make it a controllable input parameter
	setIdentity(kf.errorCovPost, cv::Scalar::all(1)); //TODO make it a controllable input parameter
	
	// initialize state vector with bounding box in [cx,cy,s,r] style
	kf.statePost.at<float>(0, 0) = initalState.x + initalState.width / 2;
	kf.statePost.at<float>(1, 0) = initalState.y + initalState.height / 2;
	kf.statePost.at<float>(2, 0) = initalState.area();
	kf.statePost.at<float>(3, 0) = initalState.width / initalState.height;
    return 0;

}