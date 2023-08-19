#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_tcp_socket.h>
#include <processor/processor.h>

#include <cxxopts.hpp>
#include <iostream>
#include <string>
#include <unordered_map>

struct RmqUriInfo {
  std::string username;
  std::string password;
  std::string host;
  int port;
  std::string exchangeName;
};

bool parseRmqUri(const std::string& rmqUri, RmqUriInfo& rmqInfo) {
  // Check for the correct URI scheme
  if (rmqUri.find("amqp://") != 0) {
    return false;
  }

  // Remove the scheme and split the remaining URI
  std::string uriWithoutScheme = rmqUri.substr(7);
  std::istringstream iss(uriWithoutScheme);
  std::getline(iss, rmqInfo.username, ':');
  std::getline(iss, rmqInfo.password, '@');
  std::getline(iss, rmqInfo.host, ':');
  iss >> rmqInfo.port;
  iss.ignore(1);  // Ignore the '/' character
  std::getline(iss, rmqInfo.exchangeName, '/');

  return true;
}

auto main(int argc, char** argv) -> int {
  cxxopts::Options options(*argv, "A program to welcome the world!");

  std::string name;
  std::string rmqUri;

  // clang-format off
  options.add_options()
    ("r,rmq", "RMQ URI", cxxopts::value(rmqUri)->default_value(""))
    ("v,video", "Video path", cxxopts::value(name)->default_value(""));
  // clang-format on

  auto result = options.parse(argc, argv);

  processor::FrameProcessor processor(name);
  std::vector<processor::FrameInfo> frameInfoList = processor.averageRGB();

  if (rmqUri.length() > 0) {
    RmqUriInfo rmqInfo;

    if (parseRmqUri(rmqUri, rmqInfo)) {
      std::cout << "Username: " << rmqInfo.username << std::endl;
      std::cout << "Password: " << rmqInfo.password << std::endl;
      std::cout << "Host: " << rmqInfo.host << std::endl;
      std::cout << "Port: " << rmqInfo.port << std::endl;
      std::cout << "Exchange Name: " << rmqInfo.exchangeName << std::endl;
    } else {
      std::cout << "Invalid RMQ URI format" << std::endl;
    }
    amqp_connection_state_t conn = amqp_new_connection();
    amqp_socket_t* socket = amqp_tcp_socket_new(conn);

    if (!socket) {
      std::cerr << "Error creating TCP socket" << std::endl;
      return 1;
    }

    int status = amqp_socket_open(socket, rmqInfo.host.c_str(), rmqInfo.port);
    if (status != AMQP_STATUS_OK) {
      std::cerr << "Error opening socket" << std::endl;
      return 1;
    }
    amqp_rpc_reply_t reply
        = amqp_login(conn, rmqInfo.host.c_str(), 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                     rmqInfo.username.c_str(), rmqInfo.password.c_str());
    if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
      std::cerr << "Login failed." << std::endl;
      return 1;
    }
    std::cerr << "Login succ!" << std::endl;
    amqp_channel_open(conn, 1);
    amqp_get_rpc_reply(conn);

    for (const processor::FrameInfo& frameInfo : frameInfoList) {
      std::string jsonLine = "{\"frame\": " + std::to_string(frameInfo.frame) + ", \"rgb\": ["
                             + std::to_string(frameInfo.rgb[0]) + ", "
                             + std::to_string(frameInfo.rgb[1]) + ", "
                             + std::to_string(frameInfo.rgb[2]) + "]}";

      // Publish JSON message to RabbitMQ
      amqp_bytes_t message_bytes = amqp_cstring_bytes(jsonLine.c_str());
      amqp_basic_publish(conn, 1, amqp_cstring_bytes(rmqInfo.exchangeName.c_str()),
                         amqp_cstring_bytes("routingKey"), 0, 0, nullptr, message_bytes);
    }

    amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
    amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
    amqp_destroy_connection(conn);

  } else {
    std::string jsonlOutput = processor.frameInfoListToJsonL(frameInfoList);
    std::cout << jsonlOutput;
  }

  return 0;
}
