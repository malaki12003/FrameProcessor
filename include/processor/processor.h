#pragma once

#include <string>
#include <vector>

namespace processor {

  struct FrameInfo {
    int frame;
    int rgb[3];
  };

  /**
   * @brief A class for proccessing video files
   */
  class FrameProcessor {
    std::string videoPath;

  public:
    /**
     * @brief Creates a new greeter
     * @param name the name to greet
     */
    FrameProcessor(std::string videoPath);

    /**
     * @brief Creates a localized string containing the greeting
     * @param lang the language to greet in
     * @return a string containing the greeting
     */
    std::vector<FrameInfo> averageRGB() const;
    std::string frameInfoListToJsonL(const std::vector<FrameInfo>& frameInfoList) const;
  };

}  // namespace processor
