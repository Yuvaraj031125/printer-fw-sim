
pipeline {
    agent {
        docker {
            image 'ubuntu:22.04'   // Use Ubuntu Docker image
            args '-u root'         // Run as root for installing packages
        }
    }

    environment {
        BUILD_DIR = 'build'
        ARTIFACT_NAME = 'printer-firmware.tar.gz'
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Install Dependencies') {
            steps {
                sh '''
                    apt-get update
                    apt-get install -y cmake g++ cppcheck libgtest-dev lcov
                    cd /usr/src/gtest
                    cmake CMakeLists.txt
                    make
                    cp lib/*.a /usr/lib
                '''
            }
        }

        stage('Build') {
            steps {
                sh '''
                    mkdir ${BUILD_DIR}
                    cd ${BUILD_DIR}
                    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" ..
                    make
                '''
            }
        }

        stage('Run Tests') {
            steps {
                sh '''
                    cd ${BUILD_DIR}
                    ctest --output-on-failure
                '''
            }
        }

        stage('Package Artifact') {
            steps {
                sh '''
                    cd ${BUILD_DIR}
                    tar -czf ${ARTIFACT_NAME} printer
                '''
            }
        }

        stage('Archive Artifact') {
            steps {
                archiveArtifacts artifacts: "${BUILD_DIR}/${ARTIFACT_NAME}", fingerprint: true
            }
        }
    }

    post {
        always {
            echo 'Pipeline finished.'
        }
    }
}
