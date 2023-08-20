#include <amqp.h>
#include <amqp_framing.h>
#include <amqp_tcp_socket.h>
#include <processor/processor.h>

#include <cxxopts.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

// Structure to hold parsed RabbitMQ URI information
struct RmqUriInfo {
  std::string username;
  std::string password;
  std::string host;
  int port;
  std::string exchangeName;
};

// Parse the provided RMQ URI
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

int main(int argc, char** argv) {
  // Set up command-line options
  cxxopts::Options options(*argv, "A program to process video frames and send data to RMQ.");

  std::string name;
  std::string rmqUri;

  options.add_options()("r,rmq", "RMQ URI", cxxopts::value(rmqUri)->default_value(""))(
      "v,video", "Video path", cxxopts::value(name)->default_value(""));

  auto result = options.parse(argc, argv);

  // Create a FrameProcessor instance to process video frames
  processor::FrameProcessor processor(name);

  try {
    // Calculate average RGB values for each frame
    std::vector<processor::FrameInfo> frameInfoList = processor.averageRGB();

    // Generate JSONL output from frame information
    std::string jsonlOutput = processor.frameInfoListToJsonL(frameInfoList);

    if (rmqUri.length() > 0) {
      RmqUriInfo rmqInfo;

      if (parseRmqUri(rmqUri, rmqInfo)) {
        // Establish a connection to RabbitMQ server
        amqp_connection_state_t conn = amqp_new_connection();
        amqp_socket_t* socket = amqp_tcp_socket_new(conn);

        if (!socket) {
          std::cerr << "Error creating TCP socket" << std::endl;
          return 1;
        }

        // Open a socket to the RabbitMQ server
        int status = amqp_socket_open(socket, rmqInfo.host.c_str(), rmqInfo.port);
        if (status != AMQP_STATUS_OK) {
          std::cerr << "Error opening socket" << std::endl;
          return 1;
        }

        // Perform authentication with the RabbitMQ server
        amqp_rpc_reply_t reply = amqp_login(conn, "/", 0, 131072, 0, AMQP_SASL_METHOD_PLAIN,
                                            rmqInfo.username.c_str(), rmqInfo.password.c_str());
        if (reply.reply_type != AMQP_RESPONSE_NORMAL) {
          std::cerr << "Login failed." << std::endl;
          return 1;
        }

        // Open a channel and get RPC reply
        amqp_channel_open(conn, 1);
        amqp_get_rpc_reply(conn);

        // Publish JSON message to RabbitMQ
        amqp_bytes_t message_bytes = amqp_cstring_bytes(jsonlOutput.c_str());
        amqp_basic_publish(conn, 1, amqp_cstring_bytes(rmqInfo.exchangeName.c_str()),
                           amqp_cstring_bytes(""), 0, 0, nullptr, message_bytes);

        // Close the channel and connection
        amqp_channel_close(conn, 1, AMQP_REPLY_SUCCESS);
        amqp_connection_close(conn, AMQP_REPLY_SUCCESS);
        amqp_destroy_connection(conn);

        std::cerr << "JSON message sent to RabbitMQ." << std::endl;
      } else {
        std::cerr << "Invalid RMQ URI format" << std::endl;
      }
    } else {
      // Print JSONL output if RMQ URI is not provided
      std::cout << jsonlOutput;
    }
  } catch (const std::exception& e) {
    std::cerr << "An error occurred: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
