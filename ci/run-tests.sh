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
fi

cp "${TEST_REPO_PATH}/spatial/snapshots/${TEST_REPO_MAP}.snapshot" "${TEST_REPO_PATH}/spatial/snapshots/default.snapshot" 

mkdir ci/${REPORT_OUTPUT_PATH}
# Create the TestResults directory if it does not exist, for storing results
New-Item -Path "$PSScriptRoot" -Name "$report_output_path" -ItemType "directory" -ErrorAction SilentlyContinue
$output_dir = "$PSScriptRoot\$report_output_path"

# We want absolute paths since paths given to the unreal editor are interpreted as relative to the UE4Editor binary
# Absolute paths are more reliable
$ue_path_absolute = Force-ResolvePath $unreal_editor_path
$uproject_path_absolute = Force-ResolvePath $uproject_path
$output_dir_absolute = Force-ResolvePath $output_dir

${UNREAL_EDITOR_PATH} \
	"${UPROJECT_PATH}" \
	"${TEST_REPO_MAP}"  \
	-execCmds="Automation RunTests ${TESTS_PATH}; Quit" \
	-TestExit="automation Test Queue Empty" \
	-ReportOutputPath="ci/${REPORT_OUTPUT_PATH}" \
	-ABSLOG="${LOG_FILE_PATH}" \
	-nopause \
	-nosplash \
	-unattended \
	-nullRHI \
	-OverrideSpatialNetworking=${RUN_WITH_SPATIAL} 
