#!/bin/bash
rproc_class_dir="/sys/class/remoteproc/remoteproc0/"

fmw_name="mpu_test_CM4.elf"
http_name="http"

if [ ! -e ./$fmw_name ]; then
    echo "$fmw_name doesnt exist"
    exit 0
fi
if [ ! -e ./$http_name ]; then
    echo "$http_name doesnt exist"
    exit 0
fi

error() {
        echo -e "$1"
        exit 0
}

#echo stop > $rproc_class_dir/state

# Copy firmware in /lib/firmware
cp /usr/local/$fmw_name /lib/firmware/

rproc_state=`tr -d '\0' < $rproc_class_dir/state`
if [ $rproc_state == "running" ]
then
echo "Stopping running fw ..."
echo stop > $rproc_class_dir/state
fi

#m4 load and start firmware
echo "Loading new firmware..."
echo $fmw_name > $rproc_class_dir/firmware
echo "Starting firmware..."
echo start > $rproc_class_dir/state

sleep 0.5

echo "Setting up Virtual COM port..."
#start virtual com
stty -onlcr -echo -F /dev/ttyRPMSG0

sleep 0.5

echo "Starting server...\n"
./http
