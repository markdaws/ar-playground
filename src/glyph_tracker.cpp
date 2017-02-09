#include "../include/glyph_tracker.h"
#include "../include/helper.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/legacy/legacy.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <vector>
#include <algorithm>

namespace ocv {

cv::Mat sub = cv::imread("./success.jpg");

GlyphTracker::GlyphTracker() :
    blurSize(cv::Point(5,5)),
    cannyMin(100),
    cannyMax(200),
    minContourArea(500),
    showMinRect(true),
    showContours(true),
    showCanny(true) {
    cv::namedWindow("input");
    cv::namedWindow("warped");

    if (showCanny) {
        cv::namedWindow("canny");
    }

    cv::namedWindow("sub");
    cv::namedWindow("gt");
}

bool isValidGlyph(cv::Mat &img2) {
    int glyph[] = {0,1,0,1,0,0,0,1,1};

    cv::Mat xxx;
    cv::cvtColor(img2, xxx, CV_BGR2GRAY);
    int code[9];
    int k = 0;
    int delta = 20;
    double yScale = xxx.rows / (double)xxx.cols;
    //std::cout << xxx.rows << ":" << xxx.cols << std::endl;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            int x = 30 + j * delta;
            int y = yScale * (30 + i * delta);

            int val = xxx.at<uchar>(y,x);
            //std::cout << "x:" << x << ", y:" << y << ", val:" << val << std::endl;
            if (val > 100) {
                code[k] = 1;
            } else {
                code[k] = 0;
            }
            //cv::circle(xxx, cv::Point(x,y), 5, cv::Scalar(255,0,0), -1);
            k++;
        }
    }

    bool matched = true;
    for (int i=0; i<9; i++) {
        if (glyph[i] != code[i]) {
            matched = false;
            break;
        }
    }
/*
    for (int i=0; i<9;i++) {
        std::cout << code[i] << ", ";
    }
    if (!matched) {
        std::cout << "not matched" << std::endl;
    } else {
        std::cout << "matched" << std::endl;
    }*/

    cv::imshow("gt", xxx);
    return matched;
}

void GlyphTracker::substituteQuad(cv::Mat &img, std::vector<cv::Point> &src) {
    auto dst = ocv::Helper::orderedPoints(src);
    double minX = std::min(dst[0].x, dst[3].x);
    double minY = std::min(dst[0].y, dst[1].y);
    double maxX = std::max(dst[1].x, dst[2].x) - minX;
    double maxY = std::max(dst[2].y, dst[3].y) - minY;

    // Move the quad to have the origin at 0,0
    for (int i=0; i<dst.size(); i++) {
        dst[i].x -= minX;
        dst[i].y -= minY;
    }

    double maxWidth, maxHeight;
    ocv::Helper::maxWidthHeight(
        dst[0], dst[1], dst[2], dst[3], maxWidth, maxHeight);

    // This is in the input quad that isn't warped by
    // persepctive
    cv::Point2f inputQuad[4];
    inputQuad[0] = cv::Point2f(0,0);
    inputQuad[1] = cv::Point2f(maxWidth-1, 0);
    inputQuad[2] = cv::Point2f(maxWidth-1, maxHeight-1);
    inputQuad[3] = cv::Point2f(0, maxHeight-1);

    // Resize the input image to the max dimensions necessary
    cv::Mat resizedSub;
    cv::resize(sub, resizedSub, cv::Size(maxWidth, maxHeight));

    // Projected quad with perspective distortion
    cv::Point2f outputQuad[4];
    outputQuad[0] = dst[0];
    outputQuad[1] = dst[1];
    outputQuad[2] = dst[2];
    outputQuad[3] = dst[3];

    cv::Mat m = cv::getPerspectiveTransform(inputQuad, outputQuad);

    cv::Mat output(maxX, maxY, CV_8UC3, cv::Scalar(0,0,0));
    cv::warpPerspective(
        resizedSub, 
        output, 
        m, 
        cv::Size(maxX, maxY),
        CV_INTER_LINEAR);

    // Render on top of the original input image
    cv::Mat imgROI = img(cv::Rect(minX,minY,maxX, maxY));
    output.copyTo(imgROI);

    cv::imshow("sub", output);
}

void GlyphTracker::getTopDownQuad(cv::Mat &img, std::vector<cv::Point> &src) {
    // Get the points in a consistent ordering
    auto srcPoints = ocv::Helper::orderedPoints(src);
    cv::Point tl = srcPoints[0];
    cv::Point tr = srcPoints[1];
    cv::Point br = srcPoints[2];
    cv::Point bl = srcPoints[3];

    // Get the max width and height of the quadrilateral
    double maxWidth, maxHeight;
    ocv::Helper::maxWidthHeight(tl, tr, br, bl, maxWidth, maxHeight);

    // Going from the input quad as seen on the screen
    cv::Point2f inputQuad[4];
    inputQuad[0] = tl;
    inputQuad[1] = tr;
    inputQuad[2] = br;
    inputQuad[3] = bl;

    // This is the target, without the perspective warping
    cv::Point2f outputQuad[4];
    outputQuad[0] = cv::Point2f(0,0);
    outputQuad[1] = cv::Point2f(maxWidth-1, 0);
    outputQuad[2] = cv::Point2f(maxWidth-1, maxHeight-1);
    outputQuad[3] = cv::Point2f(0, maxHeight-1);
    
    // Remove the perspective warping to get a consistent image
    cv::Mat m = cv::getPerspectiveTransform(inputQuad, outputQuad);
    cv::Mat output;
    cv::warpPerspective(img, output, m, cv::Size(maxWidth, maxHeight));

    // Normalize the image to always have width 100, so we can do some
    // analysis on it first
    double newWidth = 100.0;
    double scale = newWidth / output.cols;
    cv::resize(
        output, 
        output,
        cv::Size((int)newWidth, (int)(output.rows*scale)));

    //TODO:
    cv::Mat xyz;
    output.copyTo(xyz);

    cv::imshow("warped", xyz);
    if(!isValidGlyph(xyz)) {
        return;
    }

    // Render the substitute image in the perspective quad
    // of the original image
    substituteQuad(img, src);
}

void GlyphTracker::process(cv::Mat &img, cv::Mat &out) {

    cv::Mat gray;
    gray.create(img.size(), CV_8U);
    cv::cvtColor(img, gray, CV_BGR2GRAY);

    cv::GaussianBlur(gray, gray, this->blurSize, 0);
    cv::Canny(gray, gray, this->cannyMin, this->cannyMax);

    int dilation_size = 5;
    cv::Mat element = cv::getStructuringElement( cv::MORPH_RECT,
                                       cv::Size( 2*dilation_size + 1, 2*dilation_size+1 ),
                                       cv::Point( dilation_size, dilation_size ) );
    cv::dilate(gray, gray, element);
    if (this->showCanny) {
        cv::imshow("canny", gray);
    }

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(
        gray,
        contours,
        hierarchy,
        CV_RETR_TREE,
        CV_CHAIN_APPROX_SIMPLE,
        cv::Point(0,0)
    );

    // Sort and take the top 10 contours by area
    std::sort(contours.begin(), contours.end(),
        [](const std::vector<cv::Point> &a, const std::vector<cv::Point> &b) -> bool {
            return cv::contourArea(a, true) > cv::contourArea(b, true);   
        }
    );
    //contours.resize(std::min((int)contours.size(), 20));

    // Only use contours that have 4 points and can be possible
    // targets for the fudicial marker
    std::vector<std::vector<cv::Point>> quads;
    for (auto i = contours.begin(); i!=contours.end(); i++) {
        double len = cv::arcLength(*i, true);
        std::vector<cv::Point> approx;
        cv::approxPolyDP(*i, approx, 0.01*len, true);

        if ((cv::contourArea(approx, true) >= this->minContourArea) &&
            (approx.size() == 4)) {

            //std::cout << "signed area:" << cv::contourArea(approx, true) << std::endl;
        
            quads.push_back(approx);
        }
    }

    /*
    std::cout << "numquads:" << quads.size() << std::endl;
    for (int i=0; i<quads.size(); i++) {
        auto q = quads[i];
        for (int j=0; j<4; j++) {
            std::cout << "[" << q[j].x << "," << q[j].y << "], ";
        }
        std::cout << std::endl;
    }*/

    cv::RNG rng(12345);
    auto clrs = std::vector<cv::Scalar> {
        cv::Scalar(0,0,255),
        cv::Scalar(250,0,100),
        cv::Scalar(255,0,0),
        cv::Scalar(255,255,0)
    };

    for (int i=0; i<quads.size(); i++) {
        auto c = quads[i];

        if (this->showMinRect) {
            auto box = cv::minAreaRect(c);
            cv::Point2f rectPoints[4];
            box.points(rectPoints);
            for (int j=0;j<4;j++) {
                cv::line(
                    img, 
                    rectPoints[j], 
                    rectPoints[(j+1)%4], 
                    cv::Scalar(0,0,255), 
                    1, 
                    8
                );
            }

            auto op = ocv::Helper::orderedPoints(std::vector<cv::Point>{
                cv::Point(rectPoints[0].x, rectPoints[0].y),
                cv::Point(rectPoints[1].x, rectPoints[1].y),
                cv::Point(rectPoints[2].x, rectPoints[2].y),
                cv::Point(rectPoints[3].x, rectPoints[3].y)
            });
            for (int j=0; j<op.size(); j++) {
                cv::circle(img, op[j], 5, clrs[j], -1);
            }
        }

        if (this->showContours) {
            cv::Scalar color = cv::Scalar(
                rng.uniform(0, 255),
                rng.uniform(0, 255),
                rng.uniform(0, 255));
            cv::drawContours(img, quads, i, color, 2, 8, hierarchy, 0, cv::Point());
        }
        getTopDownQuad(img, c);
    }

    //out.create(img.size(), img.depth());
    //img.copyTo(out);
    cv::cvtColor(img, out, CV_BGR2GRAY);

    cv::imshow("input", img);
}

}