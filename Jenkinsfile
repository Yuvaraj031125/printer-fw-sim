
pipeline {
    agent any   // Runs on any available Jenkins node

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Install Build Tools') {
            steps {
                sh '''
                    sudo apt-get update
                    sudo apt-get install -y build-essential cmake g++
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
