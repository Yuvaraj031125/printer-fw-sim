
pipeline {
  agent { label 'ubuntu' }
  stages {
    stage('Check Git') {
      steps {
        sh 'which git'
        sh 'git --version'
      }
    }
  }
