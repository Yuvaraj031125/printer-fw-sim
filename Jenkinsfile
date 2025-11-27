pipeline {
    agent any

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
                bat '''
                    choco install cmake -y
                    choco install cppcheck -y
                    choco install mingw -y
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

        stage('Archive Artifacts') {
            steps {
                dir('printer-fw-sim\\printer-fw-sim') {
                    archiveArtifacts artifacts: 'printer-firmware.tar.gz', fingerprint: true
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
