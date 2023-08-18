#pragma once

#include <string>

namespace processor {



  /**
   * @brief A class for proccessing video files
   */
  class FrameProcessor {
    std::string name;

  public:
    /**
     * @brief Creates a new greeter
     * @param name the name to greet
     */
    FrameProcessor(std::string videoFilePath);

    /**
     * @brief Creates a localized string containing the greeting
     * @param lang the language to greet in
     * @return a string containing the greeting
     */
    std::string averageRGB() const;
  };

}  // namespace greeter
