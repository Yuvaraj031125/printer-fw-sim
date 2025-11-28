
pipeline {
  agent {
    docker {
      image 'ubuntu:24.04'
      args '-u root'
      alwaysPull true
    }
  }

  stages {
    stage('Checkout source') {
      steps {
        checkout scm
      }
    }

    stage('Install dependencies') {
      steps {
        sh '''
          set -eux
          apt-get update
          DEBIAN_FRONTEND=noninteractive apt-get install -y \
            build-essential cmake g++ cppcheck libgtest-dev lcov pkg-config

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

    stage('Build') {
      steps {
        sh '''
          set -eux
          mkdir -p build
          cd build
          cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_FLAGS="--coverage" ..
          make -j"$(nproc)"
        '''
      }
    }

    stage('Run tests') {
      steps {
        sh '''
          set -eux
          cd build
          ctest --output-on-failure
        '''
      }
    }

    stage('Package artifact') {
      steps {
        sh '''
          set -eux
          cd build
          tar -czf printer-firmware.tar.gz printer
        '''
      }
    }

    stage('Upload artifact') {
      steps {
        archiveArtifacts artifacts: 'build/printer-firmware.tar.gz', fingerprint: true
      }
    }
  }

  
  post {
    always {
      // Deletes the current workspace on the agent
      deleteDir()
    }
    success {
      echo '✅ Build, test, and packaging completed successfully (Docker agent).'
    }
    failure {
      echo '❌ Pipeline failed. Check console output.'
    }
  }
}
