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

EXPOSE 8080

CMD ["bash", "-c", "/usr/local/bin/jenkins.sh"]
