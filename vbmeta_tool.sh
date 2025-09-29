#!/bin/sh

###################################################
# Simplified version to get vbmeta digest (Android Verified Boot hash) from local device
# This won't work in device without vbmeta partition
# This script rely on python environment and avbtool from android-tools
# Root is required to run this script
# Tested on Termux
###################################################

PATH=$PATH:/data/user/0/com.termux/files/usr/bin
slot="$(getprop ro.boot.slot_suffix)"
tmp_workdir="/dev/tmp_workdir"

# run in termux: pkg install android-tools
if ! command -v avbtool; then
    echo "aborting: avbtool is not installed!"
    exit 1
fi

# Find vbmeta partition
if [ -e "/dev/block/by-name/vbmeta$slot" ]; then
    part="/dev/block/by-name/vbmeta$slot"
elif [ -e "/dev/block/bootdevice/by-name/vbmeta$slot" ]; then
    part="/dev/block/bootdevice/by-name/vbmeta$slot"
else
    echo "aborting: vbmeta partition not found!"
    exit 1
fi

mkdir -p $tmp_workdir && cd $tmp_workdir
ln -s "$part" "$tmp_workdir/vbmeta.img"

chain_partition=$(avbtool info_image --image vbmeta.img | grep -A1 "Chain Partition descriptor:" | grep "Partition Name:" | cut -d: -f2 | sed 's/ //g')

for i in $chain_partition; do
    if [ -e "/dev/block/by-name/$i$slot" ]; then
        part="/dev/block/by-name/$i$slot"
    elif [ -e "/dev/block/bootdevice/by-name/$i$slot" ]; then
        part="/dev/block/bootdevice/by-name/$i$slot"
    # Handle misc partition that has no slot suffix
    elif [ -e "/dev/block/by-name/$i" ]; then
        part="/dev/block/by-name/$i"
    elif [ -e "/dev/block/bootdevice/by-name/$i" ]; then
        part="/dev/block/bootdevice/by-name/$i"
    else
        echo "aborting: $i partition not found!"
        exit 1
    fi
    ln -s "$part" "$tmp_workdir/$i.img"
done

avbtool calculate_vbmeta_digest --image "$tmp_workdir/vbmeta.img"

cd .. && rm -rf $tmp_workdir
