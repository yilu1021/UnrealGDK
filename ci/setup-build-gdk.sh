#!/usr/bin/env bash

source /opt/improbable/environment

#TODO make parameters
GDK_HOME="$(pwd)"
GCS__PUBLISH_BUCKET="io-internal-infra-unreal-artifacts-production/UnrealEngine"
BUILD_HOME="$(pwd)/.."
UNREAL_PATH="${BUILD_HOME}/UnrealEngine"


TEST_REPO_URL="git@github.com:improbable/UnrealGDKEngineNetTest.git"
TEST_REPO_RELATIVE_UPROJECT_PATH="Game/EngineNetTest.uproject"
TEST_REPO_MAP="NetworkingMap"
TEST_PROJECT_NAME="NetworkTestProject"
CHOSEN_TEST_REPO_BRANCH="master"
SLOW_NETWORKING_TESTS=false

# Allow overriding testing branch via environment variable
if [[ -n ${TEST_REPO_BRANCH:-} ]]; then
	CHOSEN_TEST_REPO_BRANCH=${TEST_REPO_BRANCH}
fi

# Download Unreal Engine
echo "--- get-unreal-engine"
${GDK_HOME}/ci//get-engine.sh ${UNREAL_PATH}

# Run the required setup steps
echo "--- setup-gdk"
${GDK_HOME}/Setup.sh --mobile

# Build the testing project
echo "--- build-project"
${GDK_HOME}/ci/build-project.sh ${UNREAL_PATH} ${CHOSEN_TEST_REPO_BRANCH} ${TEST_REPO_URL} "${BUILD_HOME}/${TEST_PROJECT_NAME}/${TEST_REPO_RELATIVE_UPROJECT_PATH}" "${BUILD_HOME}/${TEST_PROJECT_NAME}" ${GDK_HOME} ${BUILD_PLATFORM} ${BUILD_STATE} ${BUILD_TARGET}

# TODO need to add tests back in at some point