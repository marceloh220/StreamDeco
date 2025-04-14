#!/bin/bash

sudo cp streamDeco_monitor_linux.py /usr/bin/streamdeco-monitor
sudo chmod +x /usr/bin/streamdeco-monitor
sudo mkdir /usr/share/icons/streamdeco-monitor
sudo cp streamDeco.ico /usr/share/icons/streamdeco-monitor/
sudo cp streamDeco-monitor.desktop /usr/share/applications/streamdeco-monitor.desktop
#sudo cp streamDeco-monitor.service /etc/systemd/system/streamdeco-monitor.service
#sudo systemctl daemon-reload
#sudo systemctl enable --now streamdeco-monitor.service
