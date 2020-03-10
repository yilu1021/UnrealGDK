#!/usr/bin/env bash

# TODO make those parameters!
UNREAL_PATH="${1:-$(pwd)/../UnrealEngine}"
ENGINE_CACHE_DIRECTORY="${2:-$(pwd)/UnrealEngine-Cache}"
GCS_PUBLISH_BUCKET="${3:-io-internal-infra-unreal-artifacts-production/UnrealEngine}"

GDK_HOME="$(pwd)"

pushd "${GDK_HOME}"
    # Fetch the version of Unreal Engine we need
    pushd "ci"
        # Allow overriding the engine version if required
        if [[ -n ${ENGINE_COMMIT_HASH:-} ]]; then
        	VERSION_DESCRIPTION=${ENGINE_COMMIT_HASH}
            echo "Using engine version defined by ENGINE_COMMIT_HASH: ${VERSION_DESCRIPTION}"
        else
            # Read Engine version from the file and trim any trailing white spaces and new lines.
            VERSION_DESCRIPTION=$(head -n 1 unreal-engine.version)
            echo "Using engine version found in unreal-engine.version file: ${VERSION_DESCRIPTION}"
        fi

        # Check if we are using a 'floating' engine version, meaning that we want to get the latest built version of the engine on some branch
        # This is specified by putting "HEAD name/of-a-branch" in the unreal-engine.version file
        # If so, retrieve the version of the latest build from GCS, and use that going forward.
        HEAD_VERSION_PREFIX="HEAD "
        if [[ ${VERSION_DESCRIPTION} = ${HEAD_VERSION_PREFIX}* ]]; then
        # TODO override this for now
            VERSION_BRANCH=${VERSION_DESCRIPTION#"${HEAD_VERSION_PREFIX}"} # Remove the prefix to just get the branch name
            VERSION_BRANCH= ${VERSION_BRANCH///_} # Replace / with _ since / is treated as the folder seperator in GCS
VERSION_BRANCH="feature_macos-buildkite"
            # Download the head pointer file for the given branch, which contains the latest built version of the engine from that branch
            HEAD_POINTER_GCS_PATH="gs://${GCS_PUBLISH_BUCKET}/HEAD/mac-${VERSION_BRANCH}.version"
            UNREAL_VERSION=$(gsutil cp ${HEAD_POINTER_GCS_PATH} -) # the '-' at the end instructs gsutil to download the file and output the contents to stdout
        else 
            UNREAL_VERSION=${VERSION_DESCRIPTION}
        fi
    popd

    echo ${UNREAL_VERSION}
    ## Create an UnrealEngine-Cache directory if it doesn't already exist.
    mkdir -p ${ENGINE_CACHE_DIRECTORY}

    pushd ${ENGINE_CACHE_DIRECTORY}
        echo "--- download-unreal-engine"
        ENGINE_GCS_PATH="gs://${GCS_PUBLISH_BUCKET}/${UNREAL_VERSION}.zip"
        echo "Downloading Unreal Engine artifacts version ${UNREAL_VERSION} from ${ENGINE_GCS_PATH}"
        gsutil cp -n ${ENGINE_GCS_PATH} ${UNREAL_VERSION}.zip

       7z x ${UNREAL_VERSION}.zip -o${UNREAL_VERSION} -aos
    popd

    ## Create an UnrealEngine symlink to the correct directory
    ln -s ${ENGINE_CACHE_DIRECTORY}/${UNREAL_VERSION} ${UNREAL_PATH}
popd
