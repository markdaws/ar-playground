#include "../include/video_processor.h"
#include "../include/glyph_tracker.h"

int main(int argc, char** argv) {

    ocv::VideoProcessor vp{};
    //vp.displayInput("input");
    vp.displayOutput("output");
    vp.setInput();
    vp.setFrameProcessor(std::shared_ptr<ocv::GlyphTracker>(new ocv::GlyphTracker{}));
    //vp.setFrameProcessor(std::shared_ptr<ocv::FeatureTracker>(new ocv::FeatureTracker{}));
    //vp.setFrameProcessor(std::shared_ptr<ocv::CannyProcessor>(new ocv::CannyProcessor{}));
    vp.run();
}