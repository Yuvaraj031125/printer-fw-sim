
pipeline {
  agent {
    docker {
      image 'ubuntu:24.04'
      // Prefer matching the Jenkins user uid:gid (commonly 1000:1000). Adjust if your Jenkins user differs.
      // If unsure, start with 1000:1000; you can check in the Jenkins container with `id jenkins`.
      args '-u 1000:1000'
      alwaysPull true
    }
  }

  // Disable the implicit "Declarative: Checkout SCM" that happens before stages.
  options {
    skipDefaultCheckout(true)
    timestamps()
    buildDiscarder(logRotator(numToKeepStr: '20'))
  }

  stages {
    stage('Pre-clean workspace (inside container)') {
      steps {
        sh '''
          set -eux
          # Make existing files writable, then wipe. This prevents host-side permission failures.
          chmod -R u+w . || true
          # Remove everything in workspace (files and dotfiles), but keep the workspace directory itself.
          rm -rf ./* .??* || true
        '''
      }
    }

    stage('Checkout source') {
      steps {
        // Now perform checkout; since workspace is clean, Git won't attempt its "wipe" and will clone cleanly.
        checkout scm
      }
    }

    stage('Install dependencies') {
      steps {
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
          if [ -x ./printer ]; then
            tar -czf printer-firmware.tar.gz printer
          else
            echo "Executable 'printer' not found; packaging entire build directory as fallback."
            tar -czf build-output.tar.gz .
          fi
        '''
      }
    }

    stage('Upload artifact') {
      steps {
        script {
          def hasMain = fileExists('build/printer-firmware.tar.gz')
          def hasFallback = fileExists('build/build-output.tar.gz')
          if (hasMain) {
            archiveArtifacts artifacts: 'build/printer-firmware.tar.gz', fingerprint: true
          } else if (hasFallback) {
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
      // Clean inside the container to avoid host permission issues.
      sh '''
        set -eux
        chmod -R u+w . || true
        rm -rf ./* .??* || true
      '''
    }
    success {
      echo '✅ Build, test, and packaging completed successfully (Docker agent).'
    }
    failure {
      echo '❌ Pipeline failed. Check console output.'
    }
  }
}
