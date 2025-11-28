
pipeline {
  agent {
    docker {
      image 'ubuntu:24.04'
      // Run as root so apt-get + copying to /usr/lib works
      args '-u root'
      // Optionally always pull to avoid stale images
      alwaysPull true
    }
  }

  options {
    timestamps()
    ansiColor('xterm')
  }

  stages {

    stage('Checkout source') {
      steps {
        // Requires the job to be configured as "Pipeline script from SCM"
        checkout scm
      }
    }

    stage('Install dependencies') {
      steps {
        sh '''
          set -eux
          apt-get update
          DEBIAN_FRONTEND=noninteractive apt-get install -y \
            cmake g++ cppcheck libgtest-dev lcov build-essential

          # Build GoogleTest libraries from the installed sources
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
            echo "GoogleTest sources not found; check libgtest-dev install."
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
          # Adjust 'printer' binary name if your build outputs a different target
          tar -czf printer-firmware.tar.gz printer
        '''
      }
    }

    stage('Upload artifact (archive)') {
      steps {
        // Archive to Jenkins so you can download from the build page
        archiveArtifacts artifacts: 'build/printer-firmware.tar.gz', fingerprint: true
      }
    }
  }

  post {
    success {
      echo '✅ Build, test, and packaging completed successfully (Docker agent).'
    }
    failure {
      echo '❌ Pipeline failed. Check console output and stages.'
    }
    always {
      // Optional, cleans workspace after the run
      cleanWs()
    }
  }
}
