#!/bin/bash

function setup_kaspersky_sigdb () {
    cp ./db/$1/* /opt/usr/apps/$1/data/database/arm/
    chown 5000:5000 /opt/usr/apps/$1/data/database/arm/*
    chsmack -a "$1" /opt/usr/apps/$1/data/database/arm/*
}

function setup_mcafee_sigdb () {
    rm /opt/usr/apps/$1/data/database/sdb/*
    rm /opt/usr/apps/$1/data/database/xlm/*
    cp ./db/$1/sdb/* /opt/usr/apps/$1/data/database/sdb/
    cp ./db/$1/xlm/* /opt/usr/apps/$1/data/database/xlm/
    chown 5000:5000 /opt/usr/apps/$1/data/database/sdb/2E
    chown 5000:5000 /opt/usr/apps/$1/data/database/xlm/2D
    chsmack -a "$1" /opt/usr/apps/$1/data/database/sdb/2E
    chsmack -a "$1" /opt/usr/apps/$1/data/database/xlm/2D
    return 0
}

if [ -d /opt/usr/apps/EmbkcJFK7q ]; then
    echo "MMS found"
    APPID=EmbkcJFK7q
    setup_mcafee_sigdb "EmbkcJFK7q"
elif [ -d /opt/usr/apps/docomo6004 ]; then
    echo "Anshin scan found"
    APPID=docomo6004
    setup_mcafee_sigdb "docomo6004"
elif [ -d /opt/usr/apps/nWdvlX6me3 ]; then
    echo "KIS found"
    APPID=nWdvlX6me3
    setup_kaspersky_sigdb "nWdvlX6me3"
else
    echo "Engine not found"
    exit 1
fi


echo "setup environment ..."
export XM_HOME=testcontents-1
export TCS_CONTENT_PATH=testcontents-1
export TCS_ENABLE_SCANREPAIR="yes"
export TCS_SCAN_TYPE="1"
WORKDIR=$(cd $(dirname $0) && pwd)

pushd ${WORKDIR}
echo "preparing test contents ..."
rm -rf testcontents-1
cp -rf testcontents/${APPID} testcontents-1

echo "start to run test cases ..."
./tcstest
echo "end of test cases ..."

echo "cleanup test contents ..."
rm -rf testcontents-1
popd
