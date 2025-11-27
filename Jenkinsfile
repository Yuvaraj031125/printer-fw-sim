
pipeline {
    agent any
    stages {
        stage('Hello') {
            steps {
                echo 'Hello from Jenkins!'
                script {
                    if (isUnix()) {
                        sh 'echo Running on Linux'
                    } else {
                        bat 'echo Running on Windows'
                    }
                }
            }
        }
    }
}
