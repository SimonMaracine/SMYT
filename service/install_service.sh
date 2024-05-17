#! /bin/bash

SERVICE="smyt.service"
EXECUTABLE="smytd"

mkdir --parents /opt/smyt/
cp $EXECUTABLE /opt/smyt/$EXECUTABLE
cp $SERVICE /etc/systemd/system/$SERVICE

systemctl enable $SERVICE
