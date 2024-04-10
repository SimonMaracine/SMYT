#! /bin/bash

SERVICE="smyt.service"
EXECUTABLE="smytd"

cp $SERVICE /etc/systemd/system/$SERVICE
mkdir --parents /opt/smyt/
cp $EXECUTABLE /opt/smyt/$EXECUTABLE
