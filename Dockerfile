# Use an official Python runtime as a parent image
FROM ubuntu:20.04

# Set the working directory to /app
WORKDIR /app

# Copy the current directory contents into the container at /app
COPY . /app

ARG DEBIAN_FRONTEND=noninteractive

# Install required packages
RUN apt-get update && \
    apt-get install -y git cmake g++ libopencv-dev libjsoncpp-dev librabbitmq-dev

# Compile the FrameProcessor application
RUN cmake -S standalone -B build/standalone && \
    cmake --build build/standalone



# Define environment variables
ENV RMQ_URI ""
ENV EXCHANGE_NAME ""

# Run the FrameProcessor application
CMD ["./build/standalone/Greeter", "--help"]
