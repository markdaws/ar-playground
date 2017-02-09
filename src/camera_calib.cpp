#include "../include/camera_calib.h"
#include <sstream>
#include <iostream>
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/calib3d/calib3d.hpp>

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/legacy/legacy.hpp"
#include <opencv2/video/tracking.hpp>
#include <opencv2/video/background_segm.hpp>

namespace ocv {

void CameraCalib::calibrate(
    std::vector<std::string> imgs,
    int boardW,
    int boardH,
    float squareSize
) {
    //std::ostringstream ss;
    //ss << "foo" << "what" << imgs.size();
    //std::cout << ss.str() << std::endl;

    cv::Mat img, gray;
    std::vector<std::vector<cv::Point3f>> objectPoints;
    std::vector<std::vector<cv::Point2f>> imagePoints;
    std::vector<cv::Point2f> corners;
    cv::Size boardSize = cv::Size(boardW, boardH);

    int ii=0;
    for (auto i=imgs.begin(); i!=imgs.end(); i++) {
        std::cout << *i << std::endl;

        img = cv::imread(*i, CV_LOAD_IMAGE_COLOR);
        if (!img.data) {
            std::cout << "failed to read:" << *i << std::endl;
            continue;
        }

        cv::cvtColor(img, gray, CV_BGR2GRAY);

        bool found = cv::findChessboardCorners(
            img, 
            boardSize, 
            corners, 
            CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

        if (found) {
            cv::cornerSubPix(
                gray, corners, cv::Size(5,5), cv::Size(-1, -1),
                cv::TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1)
            );
            cv::drawChessboardCorners(gray, boardSize, corners, found);

            std::ostringstream ss;
            ss << "out" << ii++ << ".jpg";
            cv::imwrite(ss.str(), gray);
        }

        std::vector<cv::Point3f> obj;
        for (int i=0; i<boardH; ++i) {
            for (int j=0; j<boardW; j++) {
                obj.push_back(cv::Point3f(
                    (float)j * squareSize,
                    (float)i * squareSize,
                    0
                ));
            }
        }

        if (found) {
            std::cout << "Found corners" << std::endl;
            imagePoints.push_back(corners);
            objectPoints.push_back(obj);
        }
    }

    cv::Mat K;
    cv::Mat D;
    std::vector<cv::Mat> rvecs, tvecs;
    int flag = 0;
    flag |= CV_CALIB_FIX_K4;
    flag |= CV_CALIB_FIX_K5;
    cv::calibrateCamera(
        objectPoints, imagePoints, img.size(), K, D, rvecs, tvecs, flag
    );

    std::cout << "K" << K;
    std::cout << "D" << D;
    std::cout << "boardW" << boardW;
    std::cout << "boardH" << boardH;

}

}