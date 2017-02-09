#include "../include/canny_processor.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/legacy/legacy.hpp"

namespace ocv {

void CannyProcessor::process(cv::Mat &img, cv::Mat &out) {
    out.create(img.size(), CV_8U);

    if (img.channels() == 3) {
        cv::cvtColor(img, out, CV_BGR2GRAY);
    }

    cv::GaussianBlur(out, out, cv::Size(9,9), 1.5);
    cv::Canny(out, out, 10, 60);
    cv::threshold(out, out, 128, 255, cv::THRESH_BINARY_INV);
}

}