#import "../include/feature_tracker.h"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/legacy/legacy.hpp"
#include <opencv2/video/tracking.hpp>

namespace ocv {

FeatureTracker::FeatureTracker() : 
    max_count(500),
    qlevel(0.01), 
    minDist(10.0) {
}

void FeatureTracker::process(cv::Mat &frame, cv::Mat &output) {
    cv::cvtColor(frame, gray, CV_BGR2GRAY);
    frame.copyTo(output);

    if(addNewPoints()) {
        detectFeaturePoints();
            
        points[0].insert(
            points[0].end(), 
            features.begin(), 
            features.end()
        );

        initial.insert(
            initial.end(),
            features.begin(),
            features.end());
    }

    if (gray_prev.empty()) {
        gray.copyTo(gray_prev);
    }

    cv::calcOpticalFlowPyrLK(
        gray_prev, gray,
        points[0],
        points[1],
        status,
        err
    );

    int k=0;
    for (int i=0; i<points[1].size();i++) {
        if (acceptTrackedPoint(i)) {
            initial[k] = initial[i];
            points[1][k++] = points[1][i];
        }
    }

    // eliminate unsuccesful points
    points[1].resize(k);
    initial.resize(k);
    // 3. handle the accepted tracked points
    handleTrackedPoints(frame, output);
    // 4. current points and image become previous ones
    std::swap(points[1], points[0]);
    cv::swap(gray_prev, gray);
}

void FeatureTracker::detectFeaturePoints() {
    // detect the features
    cv::goodFeaturesToTrack(
        gray, // the image
        features, // the output detected features
        max_count, // the maximum number of features
        qlevel, // quality level
        minDist); // min distance between two features
}

bool FeatureTracker::addNewPoints() {
    // if too few points
    return points[0].size()<=10;
}

bool FeatureTracker::acceptTrackedPoint(int i) {
    return status[i] &&
    // if point has moved
    (std::abs(points[0][i].x-points[1][i].x)+
    (std::abs(points[0][i].y-points[1][i].y))>2);
}

void FeatureTracker::handleTrackedPoints(cv:: Mat &frame,
    cv:: Mat &output) {
    // for all tracked points
    for(int i= 0; i < points[1].size(); i++ ) {
        // draw line and circle
        cv::line(output,
            initial[i], // initial position
            points[1][i],// new position
            cv::Scalar(255,255,255));
            cv::circle(output, points[1][i], 3,
            cv::Scalar(255,255,255),-1);
    }
}

}