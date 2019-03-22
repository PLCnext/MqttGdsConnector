#!/bin/bash

while getopts t:a: option
do
case "${option}"
in
t) TOOLCHAIN=${OPTARG};;
a) ARPVERSION=${OPTARG};;
esac
done

# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

# Set the root path to the PLCnext Toolchain
export PLCNEXT_TOOLCHAIN_ROOT="${TOOLCHAIN}"

cmake \
-D CMAKE_BUILD_TYPE=Release \
-D BUILD_TESTING=OFF \
-D CMAKE_STAGING_PREFIX=${DIR}/../external/deploy/axcf2152 \
-D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
-D BUILD_SHARED_LIBS=ON \
-D CMAKE_TOOLCHAIN_FILE=${PLCNEXT_TOOLCHAIN_ROOT}/toolchain.cmake \
-D ARP_TOOLCHAIN_ROOT=${PLCNEXT_TOOLCHAIN_ROOT} \
-D ARP_DEVICE=AXCF2152 \
-D "ARP_DEVICE_VERSION=${ARPVERSION}" \
-D JSON_BuildTests=OFF \
-D JSON_MultipleHeaders=OFF \
-S ${DIR}/../external/json \
-B ${DIR}/../build/external/json/axcf2152

cmake --build ${DIR}/../build/external/json/axcf2152
cmake --build ${DIR}/../build/external/json/axcf2152 --target install
