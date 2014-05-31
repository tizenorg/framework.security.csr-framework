#!/bin/bash

echo "setup environment ..."
mkdir testcontents-1
export TWP_CONTENT_PATH=testcontents-1

if [ -d /opt/usr/apps/EmbkcJFK7q ]; then
    echo "Found MMS"
    chsmack -e EmbkcJFK7q `which twptest` 
elif [ -d /opt/usr/apps/docomo6004 ]; then
    echo "Found Anshin Scan"
    chsmack -e docomo6004 `which twptest` 
else
    echo "Engine not found"
    exit 1
fi


echo "start to run test cases ..."
twptest
echo "end of test cases ..."

echo "cleanup test contents ..."
rm -rf testcontents-1

