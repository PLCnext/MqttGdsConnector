#!/bin/bash
while getopts t:a:n: option
do
case "${option}"
in
t) TOOLCHAIN=${OPTARG};;
a) ARPVERSION=${OPTARG};;
n) TARGETNAME=${OPTARG};;
esac
done

VERSION=$(echo $ARPVERSION | grep -oP [0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)
echo "Version:${VERSION}"

# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo CMAKE Configure
cmake \
-D CMAKE_BUILD_TYPE=Release \
-D BUILD_TESTING=OFF \
-DCMAKE_STAGING_PREFIX="${DIR}/../deploy/${TARGETNAME}_${VERSION}" \
-DCMAKE_INSTALL_PREFIX=/usr/local \
-DCMAKE_PREFIX_PATH="${DIR}/../deploy/${TARGETNAME}_${VERSION}" \
-D CMAKE_EXPORT_COMPILE_COMMANDS=ON \
-D BUILD_SHARED_LIBS=ON \
-D CMAKE_TOOLCHAIN_FILE="${TOOLCHAIN}/toolchain.cmake" \
-D ARP_TOOLCHAIN_ROOT="${TOOLCHAIN}" \
-D ARP_DEVICE=${TARGETNAME} \
-D "ARP_DEVICE_VERSION=${ARPVERSION}" \
-D valijson_INSTALL_HEADERS=ON \
-D valijson_BUILD_EXAMPLES=OFF \
-D valijson_BUILD_TESTS=OFF \
-S "${DIR}/../external/valijson" \
-B "${DIR}/../build/external/valijson/${TARGETNAME}_${VERSION}"

cmake --build "${DIR}/../build/external/valijson/${TARGETNAME}_${VERSION}"
cmake --build "${DIR}/../build/external/valijson/${TARGETNAME}_${VERSION}" --target install
