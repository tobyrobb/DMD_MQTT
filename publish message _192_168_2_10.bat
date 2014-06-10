@echo off
set /p input="Write something, it will be sent to the DMD  ...      "
echo "Sending mosquitto message"
cd C:\Program Files (x86)\mosquitto
mosquitto_pub.exe -h 192.168.2.10 -t DMD -m  "%input%"
echo "Sent mosquitto message"
