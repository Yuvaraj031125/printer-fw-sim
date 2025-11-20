pipeline {
    agent any

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
                sh '''
                cmake -S . -B build -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_C_FLAGS="--coverage"
                cmake --build build
                '''
            }
        }

        stage('Test') {
            steps {
                sh 'cd build && ctest --output-on-failure'
            }
        }

        stage('Static Analysis') {
            steps {
                sh 'cppcheck --enable=all src/'
            }
        }

        stage('Collect Coverage') {
            steps {
                sh '''
                lcov --capture --directory build --output-file coverage.info --ignore-errors mismatch
                lcov --remove coverage.info '/usr/*' --output-file coverage.info
                lcov --list coverage.info
                '''
                archiveArtifacts artifacts: 'coverage.info', allowEmptyArchive: true
            }
        }

        stage('Package') {
            steps {
                sh 'tar -czf printer-firmware-demo.tar.gz build/printer'
                archiveArtifacts artifacts: 'printer-firmware-demo.tar.gz', allowEmptyArchive: true
            }
        }

        stage('Docker Build & Test') {
            steps {
                sh '''
                docker build -t printer-fw-sim .
                docker run --rm printer-fw-sim
                '''
            }
        }

        stage('Docker Hub Push') {
            environment {
                DOCKERHUB_USERNAME = credentials('dockerhub-username')
                DOCKERHUB_TOKEN = credentials('dockerhub-token')
            }
            steps {
                sh '''
                echo $DOCKERHUB_TOKEN | docker login -u $DOCKERHUB_USERNAME --password-stdin
                docker tag printer-fw-sim $DOCKERHUB_USERNAME/printer-fw-sim:latest
                docker push $DOCKERHUB_USERNAME/printer-fw-sim:latest
                '''
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'printer-firmware-demo.tar.gz', fingerprint: true
            archiveArtifacts artifacts: 'coverage.info', allowEmptyArchive: true
        }
    }
}
