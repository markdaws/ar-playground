#include "../include/video_processor.h"

namespace ocv {

VideoProcessor::VideoProcessor() :
    delay(0), 
    fnumber(0), 
    stop(false) {
}

void VideoProcessor::setInput() {
    capture.open(0);
    delay = 1000.0/capture.get(CV_CAP_PROP_FPS);
}

void VideoProcessor::setFrameProcessor(std::shared_ptr<FrameProcessor> fp) {
    frameProcessor = fp;
}

void VideoProcessor::displayInput(std::string wn) {
    windowNameInput = wn;
    cv::namedWindow(windowNameInput);
}

void VideoProcessor::displayOutput(std::string wn) {
    windowNameOutput = wn;
    cv::namedWindow(windowNameOutput);
}

void VideoProcessor::run() {
    cv::Mat frame;
    cv::Mat output;

    stop = false;
    while(!stop) {
        if (!capture.read(frame)) {
            break;
        }

        if (windowNameInput != "") {
            cv::imshow(windowNameInput, frame);
        }

        if (frameProcessor) {
            frameProcessor->process(frame, output);
        } else {
            output = frame;
        }
        fnumber++;

        if (windowNameOutput != "") {
            cv::imshow(windowNameOutput, output);
        }

        //delay = 100000;
        if (cv::waitKey(delay)>=0) {
            //stop = true;
        }
    }
}

}