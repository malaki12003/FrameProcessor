#include <greeter/greeter.h>
#include <processor/processor.h>
#include <greeter/version.h>

#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

auto main(int argc, char** argv) -> int {
  const std::unordered_map<std::string, greeter::LanguageCode> languages{
      {"en", greeter::LanguageCode::EN},
      {"de", greeter::LanguageCode::DE},
      {"es", greeter::LanguageCode::ES},
      {"fr", greeter::LanguageCode::FR},
  };

  cxxopts::Options options(*argv, "A program to welcome the world!");

  std::string language;
  std::string name;

  // clang-format off
  options.add_options()
    ("v,video", "Video path", cxxopts::value(name)->default_value(""));
  // clang-format on

  auto result = options.parse(argc, argv);

  processor::FrameProcessor processor(name);
  processor.averageRGB();
  return 0;
}
