pipeline {
    agent any
    stages {
        stage('Check Docker') {
            steps {
                sh 'docker --version && docker ps'
            }
        }
    }
}
