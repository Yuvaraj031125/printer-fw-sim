
pipeline {
  agent {
    docker {
      image 'ubuntu:24.04'
      // Run as root to install packages inside the container
      args '-u root'
      // Always pull the latest tag at the start
      alwaysPull true
    }
  }

  options {
    // If you use “Pipeline script from SCM”, leave this as false OR remove it
    // If you use inline “Pipeline script”, keep skipDefaultCheckout(false) or remove entirely
    skipDefaultCheckout(false)
    timestamps()
    buildDiscarder(logRotator(numToKeepStr: '20'))
  }

  stages {
    stage('Checkout') {
      steps {
        // Works if your job is "Pipeline script from SCM" or multibranch
        checkout scm
        sh 'ls -la'
      }
    }

    stage('Install Dependencies') {
      steps {
        sh '''
          set -eux
          export DEBIAN_FRONTEND=noninteractive
          apt-get update
          apt-get install -y git build-essential cmake g++ cppcheck lcov pkg-config
        '''
      }
    }

    stage('Static Analysis') {
      steps {
        sh '''
          set -eux
          if [ -d src ]; then
            cppcheck --enable=warning,style,performance --error-exitcode=1 src
          else
            echo "No src/ directory; skipping cppcheck."
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
          cmake -DCMAKE_BUILD_TYPE=Release ..
          make -j"$(nproc)"
        '''
      }
    }

    stage('Test') {
      steps {
        sh '''
          set -eux
          if [ -f build/CTestTestfile.cmake ]; then
            cd build
            ctest --output-on-failure
          else
            echo "ctest config not found; skipping."
          fi
        '''
      }
    }

    stage('Package & Archive') {
      steps {
        sh '''
          set -eux
          cd build
          if [ -x ./app ]; then
            tar -czf app.tar.gz app
          else
            tar -czf build-output.tar.gz .
          fi
        '''
        archiveArtifacts artifacts: 'build/*.tar.gz', fingerprint: true
      }
    }
  }

  post {
    success { echo '✅ Pipeline completed successfully.' }
    failure { echo '❌ Pipeline failed. Check console output.' }
    always  { cleanWs() }
  }
}
