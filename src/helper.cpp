#include "../include/helper.h"
#include <algorithm>

namespace ocv {

// Given 4 input points of a quadrilateral, returns the maximum
// width and height of the region
void Helper::maxWidthHeight(
    const cv::Point &tl, 
    const cv::Point &tr,
    const cv::Point &br,
    const cv::Point &bl,
    double &maxWidth,
    double &maxHeight) {
        
    double w1 = sqrt(pow(br.x - bl.x, 2) + pow(br.y - bl.y, 2));
    double w2 = sqrt(pow(tr.x - tl.x, 2) + pow(tr.y - tl.y, 2));
    maxWidth = std::max(w1, w2);

    double h1 = sqrt(pow(tr.x - br.x, 2) + pow(tr.y - br.y, 2));
    double h2 = sqrt(pow(tl.x - bl.x, 2) + pow(tl.y - bl.y, 2));
    maxHeight = std::max(h1, h2);
}

// Given 4 points of a quadrilateral, returns the points in a 
// consistent ordering, from topleft, topright, bottomright, bottomleft
std::vector<cv::Point> Helper::orderedPoints(const std::vector<cv::Point> &points) {
    std::vector<cv::Point> sortedPoints(points);
    std::sort(sortedPoints.begin(), sortedPoints.end(),
    [](const cv::Point &a, const cv::Point &b) -> bool {
        return a.x < b.x;
    });
        
    std::vector<cv::Point> finalPoints(4);
    //left most 2 points, sort by y, smallest is top left, other
    //point will be bottom left
    if (sortedPoints[0].y < sortedPoints[1].y) {
        finalPoints[0] = sortedPoints[0];
        finalPoints[3] = sortedPoints[1];
    } else {
        finalPoints[0] = sortedPoints[1];
        finalPoints[3] = sortedPoints[0];
    }
    double d2 = pow(sortedPoints[2].x - finalPoints[0].x, 2) +
                pow(sortedPoints[2].y - finalPoints[0].y, 2);
    double d3 = pow(sortedPoints[3].x - finalPoints[0].x, 2) +
                pow(sortedPoints[3].y - finalPoints[0].y, 2);
    if (d2 > d3) {
        finalPoints[1] = sortedPoints[3];
        finalPoints[2] = sortedPoints[2];
    } else {
        finalPoints[1] = sortedPoints[2];
        finalPoints[2] = sortedPoints[3];
    }
    return finalPoints;
}

}