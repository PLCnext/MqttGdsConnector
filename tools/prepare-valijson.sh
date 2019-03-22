#!/bin/bash
# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

set +e
git clone -q https://github.com/tristanpenman/valijson.git "${DIR}/../external/valijson"
set -e
cd "${DIR}/../external/valijson"
git status || git init
git fetch --tags
#git checkout

