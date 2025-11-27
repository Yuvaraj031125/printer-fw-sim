
pipeline {
    agent any

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Install Dependencies') {
            steps {
                bat '''
                    choco install cmake -y
                    choco install mingw -y
                    choco install cppcheck -y
                    rem For GoogleTest, you may need to build manually or use vcpkg
                '''
            }
        }

        stage('Build') {
            steps {
                dir('printer-fw-sim\\printer-fw-sim') {
                    bat '''
                        if not exist build mkdir build
                        cd build
                        cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Debug ..
                        mingw32-make
                    '''
                }
            }
        }

        stage('Run Tests') {
            steps {
                dir('printer-fw-sim\\printer-fw-sim\\build') {
                    bat 'ctest --output-on-failure'
                }
            }
        }

        stage('Package Artifact') {
            steps {
                dir('printer-fw-sim\\printer-fw-sim') {
                    bat 'tar -czf printer-firmware.tar.gz build\\printer'
                }
            }
        }

        stage('Collect Coverage') {
            steps {
                dir('printer-fw-sim\\printer-fw-sim') {
                    bat '''
                        rem Windows alternative: use gcovr for coverage
                        pip install gcovr
                        gcovr -r . --html --html-details -o coverage-report\\index.html
                    '''
                }
            }
        }

        stage('Archive Artifacts') {
            steps {
                dir('printer-fw-sim\\printer-fw-sim') {
                    archiveArtifacts artifacts: 'printer-firmware.tar.gz, coverage-report/**/*', fingerprint: true
                }
            }
        }
    }

    post {
        always {
            cleanWs()
        }
    }
}
