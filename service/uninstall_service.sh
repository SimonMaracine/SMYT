#! /bin/bash

SERVICE="smyt.service"

rm /etc/systemd/system/$SERVICE
rm --recursive /opt/smyt/
