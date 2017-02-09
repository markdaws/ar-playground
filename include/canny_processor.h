#ifndef _OCV_CANNY_PROCESSOR_H_
#define _OCV_CANNY_PROCESSOR_H_

#include "frame_processor.h"
#include "opencv2/core/core.hpp"

namespace ocv {

class CannyProcessor : public FrameProcessor {
    public:
    void process(cv::Mat& img, cv::Mat &out);
};

}

#endif // _OCV_CANNY_PROCESSOR_H_