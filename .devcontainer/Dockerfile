FROM jenkins/jenkins:lts

USER root

# Install build tools for your C++ project
RUN apt-get update && apt-get install -y \
    cmake \
    build-essential \
    lcov \
    gcovr \
    cppcheck \
    git \
    && rm -rf /var/lib/apt/lists/*

# Switch back to Jenkins user
USER jenkins
