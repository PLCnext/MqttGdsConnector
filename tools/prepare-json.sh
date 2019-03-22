#!/bin/bash
# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

mkdir -p "${DIR}/../external"
set +e
git clone -q https://github.com/nlohmann/json.git "${DIR}/../external/json"
cd "${DIR}/../external/json"
set -e

git status || git init
git fetch --tags
git checkout v3.6.1
