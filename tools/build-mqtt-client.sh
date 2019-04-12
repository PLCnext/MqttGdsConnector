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

# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"


echo build MqttClient app with pahoC an PahoCpp
chmod +x ${DIR}/../external/mqttclient/tools/build-*.sh
${DIR}/../external/mqttclient/tools/build-all.sh -t "${TOOLCHAIN}" -a "${ARPVERSION}" -n "${TARGETNAME}"
cp -R -f ${DIR}/../external/mqttclient/deploy ${DIR}/../
