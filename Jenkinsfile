
pipeline {
  // Use the label you configured on your Ubuntu node, e.g., 'ubuntu' or 'linux'
  agent { label 'ubuntu' }

  // Polling every minute is heavy; consider every 5–15 minutes unless you truly need 1-min.
  triggers {
    pollSCM('* * * * *')
  }

  stages {

    stage('Checkout') {
      steps {
        // Works if the job has SCM configured (Pipeline from SCM or Multibranch)
        checkout scm
        // Alternative if you want to specify here:
        // git url: 'https://your.git.repo.git', branch: 'main', credentialsId: 'your-git-creds'
      }
    }

    stage('Install Dependencies') {
      steps {
        // Requires jenkins user to be able to sudo without prompt
        sh '''
          set -eux
          sudo apt-get update
          sudo apt-get install -y cmake g++ cppcheck libgtest-dev lcov

          # Build GoogleTest static libs (path can vary by distro)
          if [ -d /usr/src/gtest ]; then
            cd /usr/src/gtest
          elif [ -d /usr/src/googletest/googletest ]; then
            cd /usr/src/googletest/googletest
          else
            echo "Could not find GoogleTest sources under /usr/src; adjust path." >&2
            exit 1
          fi

          cmake CMakeLists.txt
          make -j"$(nproc)"

          # On some distros libs land under ./lib or ./lib64 after build
          if [ -d lib ]; then
            sudo cp lib/*.a /usr/lib
          elif [ -d lib64 ]; then
            sudo cp lib64/*.a /usr/lib
          else
            echo "No lib directory with built *.a found; check GTest build output." >&2
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
            # Adjust binary path 'build/printer' if your target executable has a different name
            test -f build/printer || { echo "Expected binary build/printer not found"; exit 1; }
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
            # Capture coverage
            lcov --capture --directory build --output-file coverage.info

            # Filter out system headers
            lcov --remove coverage.info '/usr/*' --output-file coverage.info

            # Print coverage summary
            lcov --list coverage.info

            # Generate HTML report
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
      // Publish JUnit XML (replace pattern to match where your tests emit results)
      script {
        // Use a shell check to avoid failing when no reports are present
        def status = sh(script: "ls printer-fw-sim/printer-fw-sim/build/**/*.xml >/dev/null 2>&1", returnStatus: true)
        if (status == 0) {
          junit testResults: 'printer-fw-sim/printer-fw-sim/build/**/*.xml', allowEmptyResults: true
        } else {
          echo 'No JUnit XML test results found to publish.'
        }
      }
      cleanWs()
    }
  }
}
