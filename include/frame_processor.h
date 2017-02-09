#ifndef _OCV_FRAMEPROCESSOR_H_
#define _OCV_FRAMEPROCESSOR_H_

#include "opencv2/core/core.hpp"

namespace ocv {
    
class FrameProcessor {
    public:
    virtual void process(cv::Mat &input, cv::Mat &output) = 0;
};

}
#endif // _OCV_FRAMEPROCESSOR_H_