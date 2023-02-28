#include "TrackerHandler.h"

TrackerHandler::TrackerHandler(double iouT, int age, std::string IPv4, std::uint16_t port){

	double iouThreshold = iouT;
    int maxAge = age;
	connection.init(IPv4,port);

}


void TrackerHandler::init(std::vector<Detection> newDetections){
    detections=newDetections;
    if (trackers.size() == 0){
                // initialize kalman trackers using first detections.
                for (unsigned int i = 0; i < detections.size(); i++)
                {
                    KalmanTracker trk;
                    trk.init(detections[i].box,idCounter);
					idCounter++;
                    trackers.push_back(trk);
                }
    }
}

void TrackerHandler::predict(){
    predictedBoxes.clear();

		for (auto it = trackers.begin(); it != trackers.end();)
		{
            if (it != trackers.end() && (*it).age > maxAge){
                 it = trackers.erase(it);
                 continue;
            }
			cv::Rect_<float> pBox = (*it).predict();
			if (pBox.x >= 0 && pBox.y >= 0)
			{
				predictedBoxes.push_back(pBox);
				it++;
			}
			else
			{
				it = trackers.erase(it);
			}
	}
}

void TrackerHandler::match(){
        int trkNum = predictedBoxes.size();
		int detNum = detections.size();

		iouMatrix.clear();
		iouMatrix.resize(trkNum, std::vector<double>(detNum, 0));

		for (unsigned int i = 0; i < trkNum; i++) // compute iou matrix as a distance matrix
		{
			for (unsigned int j = 0; j < detNum; j++)
			{
				iouMatrix[i][j] = 1 - GetIOU(predictedBoxes[i], detections[j].box);
			}
		}

		// solve the assignment problem using hungarian algorithm.
		// the resulting assignment is [track(prediction) : detection], with len=preNum
		HungarianAlgorithm HungAlgo;
		assignment.clear();

		HungAlgo.Solve(iouMatrix, assignment);

		// find matches, unmatched_detections and unmatched_predictions
		unmatchedTrajectories.clear();
		unmatchedDetections.clear();
		allItems.clear();
		matchedItems.clear();

		if (detNum >= trkNum) //	there are unmatched detections
		{
			for (unsigned int n = 0; n < detNum; n++)
				allItems.insert(n);

			for (unsigned int i = 0; i < trkNum; ++i)
				matchedItems.insert(assignment[i]);

			std::set_difference(allItems.begin(), allItems.end(),
				matchedItems.begin(), matchedItems.end(),
				insert_iterator<set<int>>(unmatchedDetections, unmatchedDetections.begin()));
		}
		else if (detNum < trkNum) // there are unmatched trajectory/predictions
		{
			for (unsigned int i = 0; i < trkNum; ++i)
				if (assignment[i] == -1) // unassigned label will be set as -1 in the assignment algorithm
					unmatchedTrajectories.insert(i);
		}

		// filter out matched with low IOU
		matchedPairs.clear();
		for (unsigned int i = 0; i < trkNum; ++i)
		{
			if (assignment[i] == -1) // pass over invalid values
				continue;
			if (1 - iouMatrix[i][assignment[i]] < iouThreshold)
			{
				unmatchedTrajectories.insert(i);
				unmatchedDetections.insert(assignment[i]);
			}
			else
				matchedPairs.push_back(cv::Point(i, assignment[i]));
		}
}

double TrackerHandler::GetIOU(cv::Rect bb_test, cv::Rect bb_gt)
{
	float in = (bb_test & bb_gt).area();
	float un = bb_test.area() + bb_gt.area() - in;

	if (un < DBL_EPSILON)
		return 0;

	return (double)(in / un);
}



std::vector<Target>  TrackerHandler::correct(){

    int detIdx, trkIdx;

    targets.clear();
	spdlog::debug("targets cleared");

    for (unsigned int i = 0; i < matchedPairs.size(); i++)
    {
        trkIdx = matchedPairs[i].x;
        detIdx = matchedPairs[i].y;

        trackers[trkIdx].correct(detections[detIdx].box);
		spdlog::debug("trackers corrected");

        trackers[trkIdx].class_id=detections[detIdx].class_id;
        trackers[trkIdx].confidence=detections[detIdx].confidence;
		spdlog::debug("targets filled");

        targets.push_back(trackers[trkIdx].getTarget());
    }

    for (auto umd : unmatchedDetections)
    {
        KalmanTracker tracker;
        tracker.init(detections[umd].box,idCounter);
		idCounter++;
        trackers.push_back(tracker);
    }
	return targets;

}

 void  TrackerHandler::projectToPlane(){
	//params = getParams(connection.getPacket())
	for(int i=0;i<trackers.size();i++){
		//trackers[i].project(params);
	}
 }; 
    
void  TrackerHandler::estimateSpeed(){

	for(int i=0;i<trackers.size();i++){
		//trackers[i].estimateSpeed();
	}

};

std::vector<Target> TrackerHandler::getTargets(){

    targets.clear();
	spdlog::debug("targets cleared");

    for (unsigned int i = 0; i < trackers.size(); i++)
    {
        targets.push_back(trackers[i].getTarget());
    }
	return targets;

}
