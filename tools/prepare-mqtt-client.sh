#!/bin/bash
# Get the directory of this script
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

set +e
git clone https://github.com/plcnext/MqttClient.git "${DIR}/../external/mqttclient"
set -e

