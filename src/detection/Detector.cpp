#include "Detector.h"



int_fast8_t Detector::forward(cv::Mat &input_image)
{
    // Convert to blob.
    cv::Mat blob;
    cv::dnn::blobFromImage(input_image, blob, 1./255., cv::Size(INPUT_WIDTH, INPUT_HEIGHT), cv::Scalar(), true, false);
 
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

    bool yolov8 = false;
    // yolov5 has an output of shape (batchSize, 25200, 85) (Num classes + box[x,y,w,h] + confidence[c])
    // yolov8 has an output of shape (batchSize, 84,  8400) (Num classes + box[x,y,w,h])
    if (dimensions > rows) // Check if the shape[2] is more than shape[1] (yolov8)
    {
        yolov8 = true;
        rows = outputs[0].size[2];
        dimensions = outputs[0].size[1];

        outputs[0] = outputs[0].reshape(1, dimensions);
        cv::transpose(outputs[0], outputs[0]);
    }
    float *data = (float *)outputs[0].data;

    for (int i = 0; i < rows; ++i)
    {
        if (yolov8)
        {
            float *classes_scores = data+4;

            cv::Mat scores(1, class_name.size(), CV_32FC1, classes_scores);
            cv::Point class_id;
            double maxClassScore;

            cv::minMaxLoc(scores, 0, &maxClassScore, 0, &class_id);

            if (maxClassScore > CONFIDENCE_THRESHOLD)
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
            }
        }
        else // yolov5
        {
            float confidence = data[4];

            if (confidence >= CONFIDENCE_THRESHOLD)
            {
                float *classes_scores = data+5;

                cv::Mat scores(1, class_name.size(), CV_32FC1, classes_scores);
                cv::Point class_id;
                double max_class_score;

                cv::minMaxLoc(scores, 0, &max_class_score, 0, &class_id);

                if (max_class_score > CONFIDENCE_THRESHOLD)
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
                }
            }
        }

        data += dimensions;
    }
    // Perform Non-Maximum Suppression and draw predictions.
    std::vector<int> indices;
    
    cv::dnn::NMSBoxes(boxes, confidences, SCORE_THRESHOLD, NMS_THRESHOLD, indices);
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
    
    float x_factor = input_image.cols / INPUT_WIDTH;
    float y_factor = input_image.rows / INPUT_HEIGHT;
    this->process(x_factor,y_factor);
    return detections;
}