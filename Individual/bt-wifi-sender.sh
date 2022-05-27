#!/bin/bash

ssid=$(cat /home/frederik/programming/sem8/test/wifi-credentials.config | grep -oP '(?<=ssid=).*')
password=$(cat /home/frederik/programming/sem8/test/wifi-credentials.config | grep -oP '(?<=pass=).*')
cat /home/frederik/programming/sem8/test/wifi-credentials.config

echo "Scanning"
scanresults=$(hcitool scan | grep -E 'SEM8ESP' | grep -oE '([0-9A-F]{2}:){5}[0-9A-F]{2}')
#scanresults="C8:C9:A3:C8:87:02"
echo "Scanned - Found: " $scanresults

echo "Looping devices"
for test in $scanresults 
do
    echo "Going over device: " $test
    echo "rfcomm release"
    $(rfcomm release 0 &> /dev/null)
    echo "rfcomm bind"
    $(rfcomm bind 0 $test 1 &> /dev/null)
    for i in {0..1}
    do
        echo "rfcomm read"
        readCommand=$(head -1 /dev/rfcomm0 | tr -d \0)
        echo "rfcomm read done"
        echo "Read command: " $readCommand
        if [[ $readCommand == *"getSSID"* ]] 
        then
            echo "Sending ssid: " $ssid
            echo $ssid > /dev/rfcomm0 
            
        elif [[ $readCommand == *"getPASS"* ]] 
        then
            echo "Sending Password: " $password
            echo $password > /dev/rfcomm0
            
        fi
    done
    $(rfcomm release 0 &> /dev/null) 
done
