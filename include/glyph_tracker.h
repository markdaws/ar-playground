#ifndef _OCV_GLYPH_TRACKER_H_
#define _OCV_GLYPH_TRACKER_H_

#include "frame_processor.h"
#include "opencv2/core/core.hpp"
#include <vector>

namespace ocv {

class GlyphTracker : public FrameProcessor {
    public:
    GlyphTracker();
    void process(cv::Mat &img, cv::Mat &out);

    private:
    cv::Point blurSize;
    int cannyMin;
    int cannyMax;
    int minContourArea;
    bool showMinRect;
    bool showContours;
    bool showCanny;

    void substituteQuad(cv::Mat &img, std::vector<cv::Point> &src);
    void getTopDownQuad(cv::Mat &img, std::vector<cv::Point> &src);
};

}

#endif // _OCV_GLYPH_TRACKER_H_