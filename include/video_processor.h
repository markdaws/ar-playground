#ifndef _OCV_VIDEO_PROCESSOR_H_
#define _OCV_VIDEO_PROCESSOR_H_

#include "frame_processor.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"

#include <memory>

namespace ocv {

class VideoProcessor {
    private:
    cv::VideoCapture capture;
    std::shared_ptr<ocv::FrameProcessor> frameProcessor;
    std::string windowNameInput;
    std::string windowNameOutput;
    int delay;
    long fnumber;
    bool stop;

    public:
    VideoProcessor();
    void setInput();
    void setFrameProcessor(std::shared_ptr<FrameProcessor> fp);
    void displayInput(std::string wn);
    void displayOutput(std::string wn);
    void run();
};

}

#endif // _OCV_VIDEO_PROCESSOR_H_