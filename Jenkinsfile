pipeline {
    agent {
        label 'ubuntu-latest'
    }
    
    triggers {
        pollSCM('* * * * *')
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
                    sudo apt-get update
                    sudo apt-get install -y cmake g++ cppcheck libgtest-dev lcov
                    cd /usr/src/gtest
                    sudo cmake CMakeLists.txt
                    sudo make
                    sudo cp lib/*.a /usr/lib
                '''
            }
        }
        
        stage('Build') {
            steps {
                dir('printer-fw-sim/printer-fw-sim') {
                    sh '''
                        mkdir -p build
                        cd build
                        cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" -DCMAKE_EXE_LINKER_FLAGS="--coverage" ..
                        make
                    '''
                }
            }
        }
        
        stage('Run Tests') {
            steps {
                dir('printer-fw-sim/printer-fw-sim/build') {
                    sh 'ctest --output-on-failure'
                }
            }
        }
        
        stage('Package Artifact') {
            steps {
                dir('printer-fw-sim/printer-fw-sim') {
                    sh 'tar -czf printer-firmware.tar.gz build/printer'
                }
            }
        }
        
        stage('Collect Coverage') {
            steps {
                dir('printer-fw-sim/printer-fw-sim') {
                    sh '''
                        lcov --capture --directory build --output-file coverage.info
                        lcov --remove coverage.info '/usr/*' --output-file coverage.info
                        lcov --list coverage.info
                        genhtml coverage.info --output-directory coverage-report
                    '''
                }
            }
        }
        
        stage('Archive Artifacts') {
            steps {
                dir('printer-fw-sim/printer-fw-sim') {
                    archiveArtifacts artifacts: 'printer-firmware.tar.gz, coverage-report/**/*', allowEmptyArchive: true
                }
            }
        }
    }
    
    post {
        always {
            script {
                if (fileExists('printer-fw-sim/printer-fw-sim/build/**/*.xml')) {
                    publishTestResults testResultsPattern: 'printer-fw-sim/printer-fw-sim/build/**/*.xml'
                }
            }
            cleanWs()
        }
    }
}
