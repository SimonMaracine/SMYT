#! /bin/bash

SERVICE="smyt.service"

systemctl disable $SERVICE

rm /etc/systemd/system/$SERVICE
rm --recursive /opt/smyt/
