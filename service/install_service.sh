#! /bin/bash

SERVICE="smyt.service"
EXECUTABLE="smyt"

cp $SERVICE /etc/systemd/system/$SERVICE
mkdir --parents /opt/smyt/
cp $EXECUTABLE /opt/smyt/$EXECUTABLE
