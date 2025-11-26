
pipeline {
    agent any

    stages {
        stage('Debug') {
            steps {
                echo "Pipeline started on ${env.NODE_NAME}"
                sh 'ls -la'
            }
        }
    }
}
