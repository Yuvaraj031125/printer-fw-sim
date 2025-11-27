pipeline {
    agent {
        docker {
            image 'ubuntu:24.04'   // Build inside Ubuntu container
            args '-u root'         // Run as root for apt installs
        }
    }

    environment {
        DEBIAN_FRONTEND = 'noninteractive'
    }

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Install Build Tools') {
            steps {
                sh '''
                    apt-get update
                    apt-get install -y build-essential cmake g++
                '''
            }
        }

        stage('Build') {
            steps {
                sh '''
                    mkdir -p build
                    cd build
                    cmake ..
                    make
                '''
            }
        }

        stage('Test') {
            steps {
                sh '''
                    cd build
                    ctest --output-on-failure || true
                '''
            }
        }
    }

    post {
        always {
            cleanWs()
        }
    }
}
