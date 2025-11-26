
pipeline {
    agent any

    triggers {
        pollSCM('H/5 * * * *')
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
                    cmake .
                    make
                    cp lib/*.a /usr/lib
                '''
            }
        }

        stage('Build') {
            steps {
                sh '''
                    mkdir -p build
                    cd build
                    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_EXE_LINKER_FLAGS="--coverage" ..
                    make
                '''
            }
        }

        stage('Run Tests') {
            steps {
                sh 'cd build && ctest --output-on-failure'
            }
        }

        stage('Package Artifact') {
            steps {
                sh 'tar -czf printer-firmware.tar.gz build/printer'
            }
        }

        stage('Collect Coverage') {
            steps {
                sh '''
                    lcov --capture --directory build --output-file coverage.info
                    lcov --remove coverage.info '/usr/*' --output-file coverage.info
                    lcov --list coverage.info
                    genhtml coverage.info --output-directory coverage-report
                '''
            }
        }

        stage('Archive Artifacts') {
            steps {
                archiveArtifacts artifacts: 'printer-firmware.tar.gz, coverage-report/**/*', allowEmptyArchive: true
            }
        }
    }

    post {
        always {
            junit 'build/**/*.xml'
            cleanWs()
        }
    }
}
