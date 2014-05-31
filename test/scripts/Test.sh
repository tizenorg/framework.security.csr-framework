#!/bin/bash

function setupsigdb () {
    rm /opt/usr/apps/$1/data/database/sdb/*
    rm /opt/usr/apps/$1/data/database/xlm/*
    cp ./db/sdb/* /opt/usr/apps/$1/data/database/sdb/
    cp ./db/xlm/* /opt/usr/apps/$1/data/database/xlm/
    chown 5000:5000 /opt/usr/apps/$1/data/database/sdb/2E
    chown 5000:5000 /opt/usr/apps/$1/data/database/xlm/2D
    chsmack -a "$1" /opt/usr/apps/$1/data/database/sdb/2E
    chsmack -a "$1" /opt/usr/apps/$1/data/database/xlm/2D
    return 0
}

echo "preparing test contents ..."
rm -rf testcontents-1
cp -rf testcontents testcontents-1

echo "setup environment ..."
export XM_HOME=testcontents-1
export TCS_CONTENT_PATH=testcontents-1
export TCS_ENABLE_SCANREPAIR="yes"
export TCS_SCAN_TYPE="1"


if [ -d /opt/usr/apps/EmbkcJFK7q ]; then
    echo "MMS found"
    setupsigdb "EmbkcJFK7q"
elif [ -d /opt/usr/apps/docomo6004 ]; then
    echo "Anshin scan found"
    setupsigdb "docomo6004"
else
    echo "Engine not found"
    exit 1
fi

echo "start to run test cases ..."
tcstest
echo "end of test cases ..."

echo "cleanup test contents ..."
rm -rf testcontents-1

