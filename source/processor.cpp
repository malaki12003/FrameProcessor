#include <fmt/format.h>
#include <opencv2/opencv.hpp>
#include <processor/processor.h>

using namespace processor;

FrameProcessor::FrameProcessor(std::string _name) : name(std::move(_name)) {}

std::string FrameProcessor::averageRGB() const {
    cv::VideoCapture cap(name);
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file" << std::endl;
        return "1";
    }

    cv::Mat frame;
    while (cap.read(frame)) {
        cv::Scalar averageColor = cv::mean(frame);
        std::cout << "Frame average RGB: "
                  << averageColor[2] << ", " << averageColor[1] << ", " << averageColor[0] << std::endl;
    }
  return "";
}
