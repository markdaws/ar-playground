#ifndef _OCV_HELPER_H_
#define _OCV_HELPER_H_

#include <math.h>
#include "opencv2/core/core.hpp"
#include <vector>

namespace ocv {

class Helper {
    public:
    static void maxWidthHeight(
        const cv::Point &tl, 
        const cv::Point &tr,
        const cv::Point &br,
        const cv::Point &bl,
        double &maxWidth,
        double &maxHeight);

    static std::vector<cv::Point> orderedPoints(
        const std::vector<cv::Point> &points
    );
};

}

#endif // _OCV_HELPER_H_