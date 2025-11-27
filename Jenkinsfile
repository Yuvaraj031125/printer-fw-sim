pipeline {
  agent { label 'ubuntu' }
  stages {
    stage('Smoke') {
      steps {
        sh '''
          set -eux
          whoami
          uname -a
          java -version
          git --version
          cmake --version || true
          g++ --version || true
          lcov --version || true
          genhtml --version || true
        '''
      }
    }
  }
}
