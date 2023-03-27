#include "Detector.h"



int_fast8_t Detector::forward(cv::Mat &input_image)
{
    // Convert to blob.
    cv::Mat blob;
    cv::dnn::blobFromImage(input_image, blob, 1./255., cv::Size(config->InputWidth, config->InputHeight), cv::Scalar(), true, false);
 
    net.setInput(blob);
 
    // Forward propagate.
    
    net.forward(outputs, net.getUnconnectedOutLayersNames());
 
    return 0;
}

int_fast8_t Detector::process(float x_factor, float y_factor)
{
    // Initialize vectors to hold respective outputs while unwrapping     detections.
    std::vector<int> class_ids;
    std::vector<float> confidences;
    std::vector<cv::Rect> boxes;
    // Resizing factor.
    
    int rows = outputs[0].size[1];
    int dimensions = outputs[0].size[2];

    if(config->yolov==YOLOV8){
        // yolov8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
        rows = outputs[0].size[2];
        dimensions = outputs[0].size[1];
        outputs[0] = outputs[0].reshape(1, dimensions);
        cv::transpose(outputs[0], outputs[0]);
        float *data = (float *)outputs[0].data;
        for (int i = 0; i < rows; ++i)
        {
            float *classes_scores = data+4;

            cv::Mat scores(1, class_name.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double maxClassScore;

            cv::minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

            if (maxClassScore > config->ConfidenceThreshold)
            {
                confidences.push_back(maxClassScore);
                class_ids.push_back(class_id.x);

                float x = data[0];
                float y = data[1];
                float w = data[2];
                float h = data[3];

                int left = int((x - 0.5 * w) * x_factor);
                int top = int((y - 0.5 * h) * y_factor);

                int width = int(w * x_factor);
                int height = int(h * y_factor);

                boxes.push_back(cv::Rect(left, top, width, height));
                data += dimensions;

            }
        }
    }else if(config->yolov==YOLOV5){
        // yolov5 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
        float *data = (float *)outputs[0].data;
        for (int i = 0; i < rows; ++i)
        {
            float confidence = data[4];

            if (confidence >= config->ConfidenceThreshold)
            {
                float *classes_scores = data+5;

                cv::Mat scores(1, class_name.size(), CV_32FC1, classes_scores);
                cv::Point class_id;
                double max_class_score;

                cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                if (max_class_score > config->ScoreThreshold)
                {
                    confidences.push_back(confidence);
                    class_ids.push_back(class_id.x);

                    float x = data[0];
                    float y = data[1];
                    float w = data[2];
                    float h = data[3];

                    int left = int((x - 0.5 * w) * x_factor);
                    int top = int((y - 0.5 * h) * y_factor);

                    int width = int(w * x_factor);
                    int height = int(h * y_factor);

                    boxes.push_back(cv::Rect(left, top, width, height));
                    data += dimensions;

                }
            }
        }
    }else if(config->yolov==YOLOV7){
        std::sort(outputs.begin(), outputs.end(), [](cv::Mat &A, cv::Mat &B) {return A.size[2] > B.size[2]; });
        float* pdata = (float*)outputs[0].data;
        int net_width = class_name.size() + 5;
        for (int stride = 0; stride < strideSize; stride++) {    //stride
		float* pdata = (float*)outputs[stride].data;
		int grid_x = (int)(config->InputWidth / netStride[stride]);
		int grid_y = (int)(config->InputHeight / netStride[stride]);
		for (int anchor = 0; anchor < 3; anchor++) {	//anchors
			const float anchor_w = netAnchors[stride][anchor * 2];
			const float anchor_h = netAnchors[stride][anchor * 2 + 1];
			for (int i = 0; i < grid_y; i++) {
				for (int j = 0; j < grid_x; j++) {
					float box_score = sigmoid_x(pdata[4]); ;
					if (box_score >= config->ConfidenceThreshold) {
						cv::Mat scores(1, class_name.size(), CV_32FC1, pdata + 5);
						cv::Point classIdPoint;
						double max_class_socre;
						minMaxLoc(scores, 0, &max_class_socre, 0, &classIdPoint);
						max_class_socre = sigmoid_x(max_class_socre);
						if (max_class_socre >= config->ScoreThreshold) {
							float x = (sigmoid_x(pdata[0]) * 2.f - 0.5f + j) * netStride[stride];  //x
							float y = (sigmoid_x(pdata[1]) * 2.f - 0.5f + i) * netStride[stride];   //y
							float w = powf(sigmoid_x(pdata[2]) * 2.f, 2.f) * anchor_w;   //w
							float h = powf(sigmoid_x(pdata[3]) * 2.f, 2.f) * anchor_h;  //h
							int left = (int)(x - 0.5 * w) * x_factor + 0.5;
							int top = (int)(y - 0.5 * h) * y_factor + 0.5;
							class_ids.push_back(classIdPoint.x);
							confidences.push_back(max_class_socre * box_score);
							boxes.push_back(cv::Rect(left, top, int(w * x_factor), int(h * y_factor)));
						}
					}
					pdata += net_width;
				}
			}
		}
	}
    }

    // Perform Non-Maximum Suppression and draw predictions.
    std::vector<int> indices;
    
    cv::dnn::NMSBoxes(boxes, confidences, config->ScoreThreshold, config->NmsThreshold, indices);
    for (int i = 0; i < indices.size(); i++)
    {
        int idx = indices[i];
        Detection detection;
        detection.box = boxes[idx];
        detection.class_id= class_ids[idx];
        detection.confidence = confidences[idx];
        detections.push_back(detection);
        
    }
    return 0;
}


std::vector<Detection> Detector::detect(cv::Mat &input_image){

    detections.clear();
    this->forward(input_image);
    
    float x_factor = input_image.cols / config->InputWidth;
    float y_factor = input_image.rows / config->InputHeight;
    this->process(x_factor,y_factor);
    return detections;
}