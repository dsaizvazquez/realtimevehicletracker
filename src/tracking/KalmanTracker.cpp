
#include "KalmanTracker.h"



int KalmanTracker::init(cv::Rect initalState,int idNum, float speedAvgFactor, int trackerId,SqlConn *con)
{
	int stateNum = 8;
	int measureNum = 4;
	id=idNum;
	kf = cv::KalmanFilter(stateNum, measureNum, 0);
	speedAveragingFactor= speedAvgFactor;

	targetDatabaseId=trackerId;
	conn=con;

	kf.transitionMatrix = (cv::Mat_<float>(stateNum, stateNum) <<
		1, 0, 0, 0, 1, 0, 0, 0,
		0, 1, 0, 0, 0, 1, 0, 0,
		0, 0, 1, 0, 0, 0, 1, 0,
		0, 0, 0, 1, 0, 0, 0, 1,
		0, 0, 0, 0, 1, 0, 0, 0,
		0, 0, 0, 0, 0, 1, 0, 0,
		0, 0, 0, 0, 0, 0, 1, 0,
		0, 0, 0, 0, 0, 0, 0, 1);

	measurement = cv::Mat::zeros(measureNum, 1, CV_32F);

	setIdentity(kf.measurementMatrix);
	setIdentity(kf.processNoiseCov, cv::Scalar::all(1e-2)); //TODO make it a controllable input parameter
	setIdentity(kf.measurementNoiseCov, cv::Scalar::all(1e-1)); //TODO make it a controllable input parameter
	setIdentity(kf.errorCovPost, cv::Scalar::all(1)); //TODO make it a controllable input parameter
	
	// initialize state vector with bounding box in [cx,cy,s,r] style
	kf.statePost.at<float>(0, 0) = initalState.x + initalState.width / 2;
	kf.statePost.at<float>(1, 0) = initalState.y + initalState.height / 2;
	kf.statePost.at<float>(2, 0) = initalState.area();
	kf.statePost.at<float>(3, 0) = initalState.width / initalState.height;
	speedVector.push_back(cv::Point3d(0,0,0));

    return 0;
}

// Predict the estimated bounding box.
cv::Rect KalmanTracker::predict()
{
	// predict
	cv::Mat p = kf.predict();

	cv::Rect predictedBox = get_rect_xysr(p.at<float>(0, 0), p.at<float>(1, 0), p.at<float>(2, 0), p.at<float>(3, 0));
	age++;
	return predictedBox;
}


// Update the state vector with observed bounding box.
int KalmanTracker::correct(cv::Rect stateMat)
{
	// measurement
	measurement.at<float>(0, 0) = stateMat.x + stateMat.width / 2;
	measurement.at<float>(1, 0) = stateMat.y + stateMat.height / 2;
	measurement.at<float>(2, 0) = stateMat.area();
	measurement.at<float>(3, 0) = stateMat.width / stateMat.height;
	// update
	kf.correct(measurement);
	age=0;
	cv::Mat s = kf.statePost;
	cv::Point2d pos(s.at<float>(0, 0),s.at<float>(1, 0));
	posPxs.push_back(pos);
	posPx = pos;
	speedPx = cv::Point2d(s.at<float>(4, 0),s.at<float>(5, 0));
	box = get_rect_xysr(s.at<float>(0, 0), s.at<float>(1, 0), s.at<float>(2, 0), s.at<float>(3, 0));

	return 0;
}


// Return the current state vector
cv::Rect KalmanTracker::getBox()
{
	return box;
}

cv::Point2d KalmanTracker::getSpeedInPx(){
	return speedPx;
}

cv::Point2d KalmanTracker::getPosInPx(){
	return posPx;
}

cv::Rect KalmanTracker::get_rect_xysr(float cx, float cy, float s, float r)
{
	float w = sqrt(s * r);
	float h = s / w;
	float x = (cx - w / 2);
	float y = (cy - h / 2);

	if (x < 0 && cx > 0)
		x = 0;
	if (y < 0 && cy > 0)
		y = 0;

	return cv::Rect(x, y, w, h);
}

TargetDetection KalmanTracker::getTarget(){
	res.box = box;
	cv::Point3d instantSpeed =speedVector[speedVector.size()-1];
	res.speed = sqrt(instantSpeed.x*instantSpeed.x+instantSpeed.y*instantSpeed.y);
    res.id = id;
	res.confidence=confidence;
    res.class_id=class_id;
	return res;
}

void KalmanTracker::project(cv::Mat K, cv::Mat R, float H,float frame_width, float frame_height){
		cv::Mat position = (cv::Mat_<float>(3,1) << posPx.x-frame_width/2, posPx.y-frame_height/2, 1);
		cv::patchNaNs(position, 0);
		cv::Mat mask = position==std::numeric_limits<float>::infinity();
		position.setTo(1000000,mask);
		posVector.push_back(projection::projectPointToFlatPlane(K,R,H,position));
} 


void KalmanTracker::estimateSpeed(float deltaTime){
	int length = posVector.size();
	if(length>2){
	cv::Point3d instantSpeed = (posVector[length-1]-posVector[length-2])/deltaTime;
	speedVector.push_back(instantSpeed*speedAveragingFactor+speedVector[speedVector.size()-1]*(1-speedAveragingFactor));
	savePosition();
	}
}

 void KalmanTracker::savePosition(){
	PositionDatabase pos{	positionCounter,
							posPx.x,
							posPx.y,
							posVector[posVector.size()-1].x,
							posVector[posVector.size()-1].y,
							speedVector[speedVector.size()-1].x,
							speedVector[speedVector.size()-1].y,
							class_id};
	positionCounter++;
	conn->insertPosition(targetDatabaseId,pos);
 }