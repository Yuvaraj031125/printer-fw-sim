
pipeline {
    agent {
        docker { image 'ubuntu:24.04' }
    }
    stages {
        stage('Build') {
            steps {
                sh 'echo "Running inside Ubuntu container"'
            }
        }
    }
}
