# Use an official Python runtime as a parent image
FROM ubuntu:20.04

# Set the working directory to /app
WORKDIR /app


ARG DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && \
    apt-get install -y git cmake g++ libopencv-dev libboost-all-dev  librabbitmq-dev doxygen

# Copy the current directory contents into the container at /app
COPY . /app

# Compile the FrameProcessor application
RUN cmake -S standalone -B build/standalone && \
    cmake --build build/standalone




# Run the FrameProcessor application

# CMD ["./build/standalone/FrameProcessor","--video", "./data/video.mp4","--rmq","amqp://guest:guest@host.docker.internal:5672/amq.direct"]
CMD ["./build/standalone/FrameProcessor","--video", "./data/video.mp4"]
# ENTRYPOINT ["tail", "-f", "/dev/null"]
