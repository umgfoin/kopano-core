#!/usr/bin/env groovy

pipeline {
    agent {
        dockerfile {
            filename 'Dockerfile.build'
        }
    }
    stages {
        stage('Build') {
            steps {
                echo 'Building..'
                sh './bootstrap.sh'
                sh 'PYTHON=/usr/bin/python3 ./configure'
                sh 'make -j $(nproc)'
            }
        }
        stage('Lint') {
            steps {
                echo 'Linting..'
                sh 'pylint3 swig/python/kopano/kopano > pylint.log || true'
                recordIssues tool: pyLint(pattern: 'pylint.log'), qualityGates: [[threshold: 1, type: 'TOTAL', unstable: true]]
            }
        }
        stage('Check') {
            steps {
                echo 'Checking..'
                sh 'make check'
            }
        }
    }
    post {
        always {
            archiveArtifacts 'pylint.log'
            cleanWs()
        }
    }
}
