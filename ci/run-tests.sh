#!/usr/bin/env bash

UNREAL_EDITOR_PATH=${1}
UPROJECT_PATH=${2}
TEST_REPO_PATH=${3}
LOG_FILE_PATH=${4}
TEST_REPO_MAP=${5}
REPORT_OUTPUT_PATH=${6}
TESTS_PATH=${7:-SpatialGDK}
RUN_WITH_SPATIAL=${8:-}

if [[ -n "${RUN_WITH_SPATIAL}" ]]; then
	echo "Generating snapshot and schema for testing project"
	"${UNREAL_EDITOR_PATH}" "${UPROJECT_PATH}" -NoShaderCompile -nopause -nosplash -unattended -nullRHI -run=GenerateSchemaAndSnapshots -MapPaths="${TEST_REPO_MAP}"
	cp "${TEST_REPO_PATH}/spatial/snapshots/${TEST_REPO_MAP}.snapshot" "${TEST_REPO_PATH}/spatial/snapshots/default.snapshot" 
fi

rm -r ci/${REPORT_OUTPUT_PATH}
mkdir ci/${REPORT_OUTPUT_PATH}

${UNREAL_EDITOR_PATH} \
	"$(pwd)/${UPROJECT_PATH}" \
	"${TEST_REPO_MAP}"  \
	-execCmds="Automation RunTests ${TESTS_PATH}; Quit" \
	-TestExit="Automation Test Queue Empty" \
	-ReportOutputPath="$(pwd)/ci/${REPORT_OUTPUT_PATH}" \
	-ABSLOG="$(pwd)/${LOG_FILE_PATH}" \
	-nopause \
	-nosplash \
	-unattended \
	-nullRHI \
	-OverrideSpatialNetworking=${RUN_WITH_SPATIAL} 
