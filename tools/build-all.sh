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
VERSION=$(echo $ARPVERSION | grep -oPe  [0-9]+[.][0-9]+[.][0-9]+[.][0-9]+)
echo "Version:${VERSION}"

# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
set -x
echo prepare Json
${DIR}/../tools/prepare-json.sh
echo prepare ValiJson
${DIR}/../tools/prepare-valijson.sh
echo prepare mqtt-client
${DIR}/../tools/prepare-mqtt-client.sh

echo build MqttClient Component with pahoC and PahoCpp
${DIR}/../tools/build-mqtt-client.sh -t "${TOOLCHAIN}" -a "${ARPVERSION}" -n "${TARGETNAME}"

echo build Json
${DIR}/../tools/build-json.sh -t "${TOOLCHAIN}" -a "${ARPVERSION}" -n "${TARGETNAME}"
echo build ValiJson
${DIR}/../tools/build-valijson.sh -t "${TOOLCHAIN}" -a "${ARPVERSION}" -n "${TARGETNAME}"
echo build GDSComponent
${DIR}/../tools/build-component.sh -t "${TOOLCHAIN}" -a "${ARPVERSION}" -n "${TARGETNAME}"
set +x


