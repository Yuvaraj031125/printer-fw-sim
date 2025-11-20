# Use official Ubuntu image
FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && \
    apt-get install -y build-essential cmake git libgtest-dev && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

# Build GTest (Ubuntu only installs sources)
RUN cd /usr/src/gtest && \
    cmake . && \
    make && \
    cp lib/*.a /usr/lib

# Set workdir
WORKDIR /app

# Copy source code
COPY . /app

# Create build directory and build project
RUN cmake -S . -B build && cmake --build build

# Run tests by default
CMD ["ctest", "--test-dir", "build"]
