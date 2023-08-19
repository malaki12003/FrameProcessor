#include <fmt/format.h>
#include <processor/processor.h>

#include <opencv2/opencv.hpp>
#include <vector>

using namespace processor;

FrameProcessor::FrameProcessor(std::string _videoPath) : videoPath(std::move(_videoPath)) {}

std::vector<FrameInfo> FrameProcessor::averageRGB() const {
  cv::VideoCapture cap(videoPath);
  if (!cap.isOpened()) {
    std::cerr << "Error opening video file" << std::endl;
    return {};
  }

  std::vector<FrameInfo> frameInfoList;

  cv::Mat frame;
  int frameNumber = 0;
  while (cap.read(frame)) {
    cv::Scalar averageColor = cv::mean(frame);
    FrameInfo frameInfo;
    frameInfo.rgb[0] = static_cast<int>(averageColor[2]);
    frameInfo.rgb[1] = static_cast<int>(averageColor[1]);
    frameInfo.rgb[2] = static_cast<int>(averageColor[0]);
    frameInfo.frame = frameNumber;
    frameInfoList.push_back(frameInfo);
    ++frameNumber;
  }

  cap.release();
  return frameInfoList;
}

std::string FrameProcessor::frameInfoListToJsonL(
    const std::vector<FrameInfo>& frameInfoList) const {
  std::string jsonLines;
  for (const FrameInfo& frameInfo : frameInfoList) {
    std::string jsonLine = "{\"frame\": " + std::to_string(frameInfo.frame) + ", \"rgb\": ["
                           + std::to_string(frameInfo.rgb[0]) + ", "
                           + std::to_string(frameInfo.rgb[1]) + ", "
                           + std::to_string(frameInfo.rgb[2]) + "]}";
    jsonLines += jsonLine + '\n';
  }
  return jsonLines;
}