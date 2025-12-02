pipeline {
    agent {
        docker {
            image 'ubuntu:22.04'
            args '-u root'
        }
    }
    
    triggers {
        pollSCM('* * * * *')
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
                    mkdir -p ${BUILD_DIR}
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
                    tar -czf ${ARTIFACT_NAME} -C ${BUILD_DIR} printer
                '''
            }
        }
        
        stage('Collect Coverage') {
            steps {
                sh '''
                    cd ${BUILD_DIR}
                    lcov --capture --directory . --output-file coverage.info
                    lcov --remove coverage.info '/usr/*' --output-file coverage_filtered.info
                    lcov --list coverage_filtered.info
                '''
            }
        }
        
        stage('Archive Artifacts') {
            steps {
                archiveArtifacts artifacts: "${ARTIFACT_NAME}, ${BUILD_DIR}/coverage*.info", allowEmptyArchive: true
            }
        }
    }
    
    post {
        always {
            script {
                if (fileExists("${BUILD_DIR}/**/*.xml")) {
                    publishTestResults testResultsPattern: "${BUILD_DIR}/**/*.xml"
                }
            }
            cleanWs()
            echo 'Pipeline finished.'
        }
    }
}
