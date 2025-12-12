pipeline {
    agent {
        docker {
            image 'ubuntu:22.04'
            args '-u root'
        }
    }
    
    environment {
        BUILD_DIR = 'build'
        ARTIFACT_NAME = 'printer-firmware.tar.gz'
        CMAKE_BUILD_TYPE = 'Debug'
        ENABLE_COVERAGE = 'ON'
    }
    
    triggers {
        // Equivalent to GitHub Actions push/pull_request on main branch
        pollSCM('H/5 * * * *') // Poll every 5 minutes for changes
        // Alternative: Use GitHub webhook for immediate triggering
        // githubPush()
    }
    
    options {
        buildDiscarder(logRotator(numToKeepStr: '10'))
        timeout(time: 30, unit: 'MINUTES')
        timestamps()
        skipDefaultCheckout(false)
    }
    
    stages {
        stage('Checkout') {
            steps {
                echo 'Checking out source code...'
                checkout scm
                sh 'ls -la'
                sh 'pwd'
            }
        }
        
        stage('Install Dependencies') {
            steps {
                echo 'Installing system dependencies...'
                sh '''
                    apt-get update && apt-get install -y \
                        cmake \
                        g++ \
                        cppcheck \
                        libgtest-dev \
                        lcov \
                        build-essential \
                        git \
                        && apt-get clean \
                        && rm -rf /var/lib/apt/lists/*
                '''
                
                echo 'Building GoogleTest libraries...'
                sh '''
                    cd /usr/src/gtest && \
                    cmake . && \
                    make && \
                    cp lib/*.a /usr/lib/ || cp *.a /usr/lib/
                '''
                
                echo 'Verifying installations...'
                sh 'cmake --version'
                sh 'g++ --version'
                sh 'cppcheck --version'
                sh 'lcov --version'
            }
        }
        
        stage('Static Code Analysis') {
            steps {
                echo 'Running static code analysis...'
                sh '''
                    cppcheck --enable=all --xml --xml-version=2 \
                        --suppress=missingIncludeSystem \
                        --suppress=unusedFunction \
                        src/ tests/ 2> cppcheck-report.xml || true
                '''
                
                // Archive the cppcheck report
                archiveArtifacts artifacts: 'cppcheck-report.xml', allowEmptyArchive: true
            }
        }
        
        stage('Build') {
            steps {
                echo 'Creating build directory and configuring project...'
                sh 'mkdir -p ${BUILD_DIR}'
                
                echo 'Running CMake configuration...'
                sh '''
                    cmake -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
                          -DENABLE_COVERAGE=${ENABLE_COVERAGE} \
                          -DCMAKE_CXX_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
                          -DCMAKE_C_FLAGS="--coverage -fprofile-arcs -ftest-coverage" \
                          -DCMAKE_EXE_LINKER_FLAGS="--coverage" \
                          -B${BUILD_DIR} .
                '''
                
                echo 'Building the project...'
                sh 'cmake --build ${BUILD_DIR} --parallel $(nproc)'
                
                echo 'Verifying build artifacts...'
                sh 'ls -la ${BUILD_DIR}/'
                sh 'file ${BUILD_DIR}/printer || echo "Main executable not found"'
                sh 'file ${BUILD_DIR}/test_printer || echo "Test executable not found"'
            }
        }
        
        stage('Run Tests') {
            steps {
                echo 'Executing unit tests...'
                sh '''
                    cd ${BUILD_DIR} && \
                    ctest --output-on-failure --verbose --parallel $(nproc)
                '''
                
                echo 'Running tests directly for additional verification...'
                sh '''
                    cd ${BUILD_DIR} && \
                    ./test_printer --gtest_output=xml:test-results.xml || true
                '''
            }
            post {
                always {
                    // Publish test results if available
                    publishTestResults testResultsPattern: '${BUILD_DIR}/**/*.xml', allowEmptyResults: true
                }
            }
        }
        
        stage('Code Coverage Analysis') {
            steps {
                echo 'Capturing code coverage data...'
                sh '''
                    lcov --capture \
                         --directory ${BUILD_DIR} \
                         --output-file coverage.info \
                         --rc lcov_branch_coverage=1
                '''
                
                echo 'Filtering coverage data...'
                sh '''
                    lcov --remove coverage.info \
                         '/usr/*' \
                         '*/tests/*' \
                         '*/test_*' \
                         --output-file coverage-filtered.info \
                         --rc lcov_branch_coverage=1
                '''
                
                echo 'Generating coverage summary...'
                sh 'lcov --list coverage-filtered.info'
                
                echo 'Generating HTML coverage report...'
                sh '''
                    genhtml coverage-filtered.info \
                            --output-directory coverage-html \
                            --title "Printer Firmware Coverage Report" \
                            --num-spaces 4 \
                            --rc lcov_branch_coverage=1
                '''
            }
            post {
                always {
                    // Archive coverage reports
                    archiveArtifacts artifacts: 'coverage*.info,coverage-html/**/*', allowEmptyArchive: true
                    
                    // Publish HTML coverage report
                    publishHTML([
                        allowMissing: false,
                        alwaysLinkToLastBuild: true,
                        keepAll: true,
                        reportDir: 'coverage-html',
                        reportFiles: 'index.html',
                        reportName: 'Coverage Report'
                    ])
                }
            }
        }
        
        stage('Package Artifact') {
            steps {
                echo 'Creating firmware artifact package...'
                sh '''
                    # Verify the executable exists
                    if [ -f "${BUILD_DIR}/printer" ]; then
                        echo "Packaging printer executable..."
                        tar -czf ${ARTIFACT_NAME} \
                            -C ${BUILD_DIR} printer \
                            --transform 's,^,printer-firmware/,'
                    else
                        echo "Warning: printer executable not found, creating empty package"
                        mkdir -p temp-package/printer-firmware
                        echo "Printer executable not built successfully" > temp-package/printer-firmware/README.txt
                        tar -czf ${ARTIFACT_NAME} -C temp-package .
                        rm -rf temp-package
                    fi
                '''
                
                echo 'Verifying package contents...'
                sh 'tar -tzf ${ARTIFACT_NAME}'
                sh 'ls -lh ${ARTIFACT_NAME}'
            }
        }
        
        stage('Archive Artifacts') {
            steps {
                echo 'Archiving build artifacts...'
                archiveArtifacts artifacts: '''
                    ${ARTIFACT_NAME},
                    coverage*.info,
                    ${BUILD_DIR}/**/*.xml,
                    cppcheck-report.xml
                ''', allowEmptyArchive: true, fingerprint: true
                
                echo 'Artifacts archived successfully'
            }
        }
    }
    
    post {
        always {
            echo 'Publishing test results...'
            junit testResultsPattern: '${BUILD_DIR}/**/*.xml', allowEmptyResults: true
            
            echo 'Cleaning workspace...'
            cleanWs()
            
            echo 'Pipeline finished.'
        }
        
        success {
            echo 'Pipeline completed successfully!'
            // Add notification logic here (email, Slack, etc.)
        }
        
        failure {
            echo 'Pipeline failed!'
            // Add failure notification logic here
        }
        
        unstable {
            echo 'Pipeline completed with warnings'
            // Add unstable notification logic here
        }
    }
}
