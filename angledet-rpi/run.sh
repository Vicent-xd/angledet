#!/bin/bash
file="/home/pi/angledet/data"
if [ -f "$file" ]; then
  rm "$file"
  echo "$file removed" 
fi
/home/pi/angledet/angledet > /tmp/angledet.log&
sleep 5s
python3.7 /home/pi/angledet/plot.py
pkill angledet
