#ifndef DRAWING
#define DRAWING
#include <opencv2/opencv.hpp>

// Text parameters.
const float FONT_SCALE = 0.45;
const int FONT_FACE = cv::FONT_HERSHEY_SIMPLEX;
const int THICKNESS = 1;
 
// Colors.
cv::Scalar BLACK = cv::Scalar(0,0,0);
cv::Scalar WHITE = cv::Scalar(255,255,255);

cv::Scalar BLUE = cv::Scalar(255, 178, 50);
cv::Scalar YELLOW = cv::Scalar(0, 255, 255);
cv::Scalar RED = cv::Scalar(0,0,255);


namespace draw{
    void draw_label(cv::Mat& input_image, std::string label, int left, int top)
    {
        // Display the label at the top of the bounding box.
        int baseLine;
        cv::Size label_size = cv::getTextSize(label, FONT_FACE, FONT_SCALE, THICKNESS, &baseLine);
        top = std::max(top, label_size.height);
        // Top left corner.
        cv::Point tlc = cv::Point(left, top);
        // Bottom right corner.
        cv::Point brc = cv::Point(left + label_size.width, top + label_size.height + baseLine);
        // Draw white rectangle.
        rectangle(input_image, tlc, brc, WHITE, cv::FILLED);
        // Put the label on the black rectangle.
        putText(input_image, label, cv::Point(left, top + label_size.height), FONT_FACE, FONT_SCALE, BLACK, THICKNESS);
    }
} 

#endif