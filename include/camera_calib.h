#ifndef _OCV_CAMERA_CALIB_H_
#define _OCV_CAMERA_CALIB_H_

#include <vector>

namespace ocv {

class CameraCalib {
    public:
    void calibrate(
        std::vector<std::string> imgs,
        int boardW,
        int boardH,
        float squareSize
        );
};

}

#endif //_OCV_CAMERA_CALIB_H_