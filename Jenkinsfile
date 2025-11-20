pipeline {
    agent any

    stages {
        stage('Checkout') {
            steps { checkout scm }
        }

        stage('Build') {
            steps {
                sh '''
                mkdir -p build
                cd build
                cmake ..
                make
                '''
            }
        }

        stage('Test') {
            steps {
                sh 'cd build && ctest --output-on-failure'
            }
        }

        stage('Static Analysis') {
            steps {
                sh 'cppcheck --enable=all src/'
            }
        }

        stage('Package') {
            steps {
                sh 'tar -czf printer-firmware-demo.tar.gz build/printer'
            }
        }
    }

    post {
        always {
            archiveArtifacts artifacts: 'printer-firmware-demo.tar.gz', fingerprint: true
        }
    }
}
