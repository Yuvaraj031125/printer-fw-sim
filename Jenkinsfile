
pipeline {
  // Run on a Docker-capable agent node instead of using a controller-side docker agent
  agent { label 'docker-node' }

  options {
    skipDefaultCheckout(true)
    timestamps()
    buildDiscarder(logRotator(numToKeepStr: '20'))
    // If you ever parallelize, consider: parallelsAlwaysFailFast()
  }

  stages {
    stage('Prepare Workspace') {
      steps {
        // Requires Workspace Cleanup plugin
        cleanWs(deleteDirs: true, notFailBuild: true, disableDeferredWipeout: true)
      }
    }

    stage('Unpack & Checkout') {
      steps {
        checkout scm
        sh 'test -f CMakeLists.txt || { echo "Missing CMakeLists.txt"; exit 1; }'
      }
    }

    stage('Install Dependencies (inside docker)') {
      steps {
        script {
          def img = docker.image('ubuntu:24.04')
          // Always pull to simulate `alwaysPull true`
          img.pull()
          img.inside('-u root') {
            sh '''
              set -eux
              apt-get update
              DEBIAN_FRONTEND=noninteractive apt-get install -y \
                build-essential cmake g++ cppcheck libgtest-dev lcov pkg-config git

              # Build GoogleTest libraries
              if [ -d /usr/src/googletest/googletest ]; then
                cd /usr/src/googletest/googletest
                cmake -S . -B build
                cmake --build build -j"$(nproc)"
                cp build/lib/*.a /usr/lib
              elif [ -d /usr/src/gtest ]; then
                cd /usr/src/gtest
                cmake CMakeLists.txt
                make -j"$(nproc)"
                cp lib/*.a /usr/lib
              else
                echo "GoogleTest sources not found; check libgtest-dev."
                exit 1
              fi
            '''
          }
        }
      }
    }

    stage('Static Analysis (inside docker)') {
      steps {
        script {
          def img = docker.image('ubuntu:24.04')
          img.inside('-u root') {
            sh '''
              set -eux
              if [ -d src ]; then
                cppcheck --enable=warning,style,performance --error-exitcode=1 src || {
                  echo "cppcheck found issues"; exit 1;
                }
              else
                echo "No src/ directory; skipping cppcheck."
              fi
            '''
          }
        }
      }
    }

    stage('Build (inside docker)') {
      steps {
        script {
          def img = docker.image('ubuntu:24.04')
          img.inside('-u root') {
            sh '''
              set -eux
              mkdir -p build
              cd build
              cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" ..
              make -j"$(nproc)"
            '''
          }
        }
      }
    }

    stage('Run Tests (inside docker)') {
      steps {
        script {
          def img = docker.image('ubuntu:24.04')
          img.inside('-u root') {
            sh '''
              set -eux
              cd build
              ctest --output-on-failure
            '''
          }
        }
      }
    }

    stage('Package Artifact (inside docker)') {
      steps {
        script {
          def img = docker.image('ubuntu:24.04')
          img.inside('-u root') {
            sh '''
              set -eux
              cd build
              if [ -x ./printer ]; then
                tar -czf printer-firmware.tar.gz printer
              else
                echo "Executable 'printer' not found; packaging entire build directory."
                tar -czf build-output.tar.gz .
              fi
            '''
          }
        }
      }
    }

    stage('Archive Artifact') {
      steps {
        script {
          def mainArtifact = fileExists('build/printer-firmware.tar.gz')
          def fallbackArtifact = fileExists('build/build-output.tar.gz')
          if (mainArtifact) {
            archiveArtifacts artifacts: 'build/printer-firmware.tar.gz', fingerprint: true
          } else if (fallbackArtifact) {
            archiveArtifacts artifacts: 'build/build-output.tar.gz', fingerprint: true
          } else {
            error('No artifact found to archive.')
          }
        }
      }
    }
  }

  post {
    always {
      // Guard against missing workspace context if agent setup fails early
      script {
        if (env.WORKSPACE) {
          cleanWs(deleteDirs: true, notFailBuild: true, disableDeferredWipeout: true)
        } else {
          echo 'No workspace context available; skipping cleanWs.'
        }
      }
    }
    success {
      echo '✅ Build, test, and packaging completed successfully.'
    }
    failure {
      echo '❌ Pipeline failed. Check console output.'
    }
  }
}
