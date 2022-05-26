#!/bin/bash

ssid=$(cat ./wifi-credentials.config | grep -oP '(?<=ssid=).*')
pass=$(cat ./wifi-credentials.config | grep -oP '(?<=pass=).*')

scanresults=$(hcitool scan | grep -E 'SEM8ESP' | grep -oE '([0-9A-F]{2}:){5}[0-9A-F]{2}')
echo $scanresults

for test in $scanresults 
do
    $(rfcomm release 0 &> /dev/null)
    $(rfcomm bind 0 $test 1 &> /dev/null)
    readCommand=$(tail -1 /dev/rfcomm0 | tr -d \0)
    if [[ $readCommand == *"getSSID"* ]] 
    then
        echo $ssid > /dev/rfcomm0
    elif [[ $readCommand == *"getPASS"* ]] 
    then
        echo $pass > /dev/rfcomm0
    fi
    $(rfcomm release 0 &> /dev/null) 
done