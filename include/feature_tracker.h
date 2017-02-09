#ifndef _OCV_FEATURE_TRACKER_H_
#define _OCV_FEATURE_TRACKER_H_

#include "frame_processor.h"
#include "opencv2/core/core.hpp"
#include <vector>

namespace ocv {

class FeatureTracker: public ocv::FrameProcessor {
    private:
    cv::Mat gray;
    cv::Mat gray_prev;
    std::vector<cv::Point2f> points[2];
    std::vector<cv::Point2f> initial;
    std::vector<cv::Point2f> features;
    int max_count;
    double qlevel;
    double minDist;
    std::vector<uchar> status;
    std::vector<float> err;

    public:
    FeatureTracker();
    void process(cv::Mat &frame, cv::Mat &output);

    private:
    bool addNewPoints();
    bool acceptTrackedPoint(int i);
    void detectFeaturePoints();
    void handleTrackedPoints(cv:: Mat &frame, cv:: Mat &output);
};

}

#endif // _OCV_FRATURE_TRACKER_H_