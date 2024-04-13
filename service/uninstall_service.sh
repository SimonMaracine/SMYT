#! /bin/bash

SERVICE="smyt.service"

systemctl disable smyt.service

rm /etc/systemd/system/$SERVICE
rm --recursive /opt/smyt/
