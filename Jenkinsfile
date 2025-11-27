
pipeline {
    agent { label 'ubuntu' }  // Make sure this matches your Ubuntu node label

    triggers {
        pollSCM('H/5 * * * *') // Poll every 5 minutes (adjust as needed)
    }

    stages {
        stage('Agent Sanity Check') {
            steps {
                sh '''
                    set -eux
                    echo "Running on: $(uname -a)"
                    echo "User: $(whoami)"
                    java -version
                    git --version
                '''
            }
        }

        stage('Checkout') {
            steps {
                checkout scm
                // If not using Pipeline from SCM, use:
                // git url: 'https://your.git.repo.git', branch: 'main', credentialsId: 'your-git-creds'
            }
        }

        stage('Install Dependencies') {
            steps {
                sh '''
                    set -eux
                    sudo apt-get update
                    sudo apt-get install -y cmake g++ cppcheck libgtest-dev lcov genhtml

                    # Build GoogleTest static libs
                    if [ -d /usr/src/gtest ]; then
                        cd /usr/src/gtest
                    elif [ -d /usr/src/googletest/googletest ]; then
                        cd /usr/src/googletest/googletest
                    else
                        echo "GoogleTest sources not found under /usr/src" >&2
                        exit 1
                    fi

                    cmake CMakeLists.txt
                    make -j"$(nproc)"

                    if [ -d lib ]; then
                        sudo cp lib/*.a /usr/lib
                    elif [ -d lib64 ]; then
                        sudo cp lib64/*.a /usr/lib
                    else
                        echo "No lib directory found after building GoogleTest" >&2
                        exit 1
                    fi
                '''
            }
        }

        stage('Build') {
            steps {
                dir('printer-fw-sim/printer-fw-sim') {
                    sh '''
                        set -eux
                        mkdir -p build
                        cd build
                        cmake -DCMAKE_BUILD_TYPE=Debug \
                              -DCMAKE_CXX_FLAGS="--coverage" \
                              -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
                              ..
                        make -j"$(nproc)"
                    '''
                }
            }
        }

        stage('Run Tests') {
            steps {
                dir('printer-fw-sim/printer-fw-sim/build') {
                    sh '''
                        set -eux
                        ctest --output-on-failure
                    '''
                }
            }
        }

        stage('Package Artifact') {
            steps {
                dir('printer-fw-sim/printer-fw-sim') {
                    sh '''
                        set -eux
                        test -f build/printer || { echo "Binary build/printer not found" >&2; exit 1; }
                        tar -czf printer-firmware.tar.gz build/printer
                    '''
                }
            }
        }

        stage('Collect Coverage') {
            steps {
                dir('printer-fw-sim/printer-fw-sim') {
                    sh '''
                        set -eux
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
                    archiveArtifacts artifacts: 'printer-firmware.tar.gz, coverage-report/**/*', fingerprint: true
                }
            }
        }
    }

    post {
        always {
            script {
                // Publish JUnit XML if available
                def status = sh(script: "ls printer-fw-sim/printer-fw-sim/build/**/*.xml >/dev/null 2>&1", returnStatus: true)
                if (status == 0) {
                    junit testResults: 'printer-fw-sim/printer-fw-sim/build/**/*.xml', allowEmptyResults: true
                } else {
                    echo 'No JUnit XML test results found.'
                }
            }
            cleanWs()
        }
    }
}
