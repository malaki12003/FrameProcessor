#include <fmt/format.h>
#include <processor/processor.h>

#include <opencv2/opencv.hpp>
#include <vector>

using namespace processor;

// Constructor to initialize the video path
FrameProcessor::FrameProcessor(std::string _videoPath) : videoPath(std::move(_videoPath)) {}

// Calculate average RGB values for each frame of the video
std::vector<FrameInfo> FrameProcessor::averageRGB() const {
  std::vector<FrameInfo> frameInfoList;

  try {
    // Open the video file
    cv::VideoCapture cap(videoPath);
    if (!cap.isOpened()) {
      std::cerr << "Error opening video file" << std::endl;
      return frameInfoList;
    }

    cv::Mat frame;
    int frameNumber = 1;
    while (cap.read(frame)) {
      // Calculate the average color of the frame
      cv::Scalar averageColor = cv::mean(frame);

      // Create a FrameInfo object to store frame information
      FrameInfo frameInfo;
      frameInfo.rgb[0] = static_cast<int>(averageColor[2]);
      frameInfo.rgb[1] = static_cast<int>(averageColor[1]);
      frameInfo.rgb[2] = static_cast<int>(averageColor[0]);
      frameInfo.frame = frameNumber;

      // Add the FrameInfo object to the list
      frameInfoList.push_back(frameInfo);
      ++frameNumber;
    }

    // Release the video capture object
    cap.release();
  } catch (const cv::Exception& e) {
    std::cerr << "OpenCV Exception: " << e.what() << std::endl;
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return frameInfoList;
}

// Convert a list of FrameInfo objects to JSONL format
std::string FrameProcessor::frameInfoListToJsonL(
    const std::vector<FrameInfo>& frameInfoList) const {
  std::string jsonLines;

  try {
    for (const FrameInfo& frameInfo : frameInfoList) {
      // Create a JSON line for each FrameInfo object
      std::string jsonLine = fmt::format(R"({{"frame": {}, "rgb": [{}, {}, {}]}})", frameInfo.frame,
                                         frameInfo.rgb[0], frameInfo.rgb[1], frameInfo.rgb[2]);

      // Add the JSON line to the accumulated JSONL output
      jsonLines += jsonLine + '\n';
    }
  } catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }

  return jsonLines;
}
