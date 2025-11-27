
pipeline {
  agent {
    docker {
      // Build container image; you can pin a specific digest for reproducibility
      image 'ubuntu:24.04'
      // Run as root to use apt-get
      args '-u root --network host'
    }
  }

  environment {
    // Noninteractive apt to avoid tzdata prompts
    DEBIAN_FRONTEND = 'noninteractive'
  }

  stages {
    stage('Checkout') {
      steps {
        checkout scm
        sh 'git --version || true'
      }
    }

    stage('Install Dependencies') {
      steps {
        sh '''
          set -euxo pipefail
          apt-get update
          apt-get install -y --no-install-recommends \
            build-essential cmake g++ cppcheck \
            libgtest-dev lcov git ca-certificates

          # Build and install GoogleTest static libraries
          cd /usr/src/googletest
          cmake -S . -B build
          cmake --build build -j$(nproc)
          cp build/lib/*.a /usr/lib

          # Ensure certificates are up-to-date (helps with git/HTTPS)
          update-ca-certificates || true
        '''
      }
    }

    stage('Build') {
      steps {
        dir('printer-fw-sim/printer-fw-sim') {
          sh '''
            set -euxo pipefail
            rm -rf build
            mkdir -p build
            cd build
            cmake -DCMAKE_BUILD_TYPE=Debug \
                  -DCMAKE_CXX_FLAGS="--coverage" \
                  -DCMAKE_EXE_LINKER_FLAGS="--coverage" ..
            make -j$(nproc)
          '''
        }
      }
    }

    stage('Run Tests') {
      steps {
        dir('printer-fw-sim/printer-fw-sim/build') {
          sh '''
            set -euxo pipefail
            # If ctest produces JUnit XML, keep them under build/ for publishing
            ctest --output-on-failure
          '''
        }
      }
    }

    stage('Package Artifact') {
      steps {
        dir('printer-fw-sim/printer-fw-sim') {
          sh '''
            set -euxo pipefail
            test -f build/printer
            tar -czf printer-firmware.tar.gz -C build printer
          '''
        }
      }
    }

    stage('Collect Coverage') {
      steps {
        dir('printer-fw-sim/printer-fw-sim') {
          sh '''
            set -euxo pipefail
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
      // Publish JUnit results if they exist (adjust pattern if your tests write XML reports)
      junit testResults: 'printer-fw-sim/printer-fw-sim/build/**/*.xml', allowEmptyResults: true
      cleanWs()
    }
  }
}
