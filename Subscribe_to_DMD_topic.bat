@echo off
echo "Subscribing to DMD  ...  "
cd C:\Program Files (x86)\mosquitto
mosquitto_sub.exe -h 192.168.2.10 -t "DMD"
