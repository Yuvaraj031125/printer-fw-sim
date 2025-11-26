
pipeline {
    agent any

    parameters {
        string(name: 'BUILD_TYPE', defaultValue: 'Debug', description: 'Choose build type')
        booleanParam(name: 'RUN_DOCKER', defaultValue: true, description: 'Run Docker steps?')
    }

    stages {
        stage('Checkout') {
            steps { checkout scm }
        }

        stage('Install Dependencies') {
            steps {
                sh '''
                sudo apt-get update
                sudo apt-get install -y cmake g++ cppcheck libgtest-dev lcov docker.io
                cd /usr/src/gtest
                sudo cmake .
                sudo make
                sudo cp lib/*.a /usr/lib
                '''
            }
        }

        stage('Build') {
            steps {
                sh """
                cmake -S . -B build -DCMAKE_BUILD_TYPE=${params.BUILD_TYPE} -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_C_FLAGS="--coverage"
                cmake --build build
                """
            }
        }

        stage('Docker Build & Test') {
            when {
                expression { params.RUN_DOCKER }
            }
            steps {
                sh '''
                docker build -t printer-fw-sim .
                docker run --rm printer-fw-sim
                '''
            }
        }
    }
}
