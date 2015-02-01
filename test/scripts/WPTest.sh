#!/bin/bash

echo "setup environment ..."
WORKDIR=$(cd $(dirname $0) && pwd)

if [ -d /opt/usr/apps/EmbkcJFK7q ]; then
    echo "Found MMS"
    chsmack -e EmbkcJFK7q `which ${WORKDIR}/twptest` 
elif [ -d /opt/usr/apps/docomo6004 ]; then
    echo "Found Anshin Scan"
    chsmack -e docomo6004 `which ${WORKDIR}/twptest` 
elif [ -d /opt/usr/apps/nWdvlX6me3 ]; then
    echo "Found KIS"
    chsmack -e nWdvlX6me3 `which ${WORKDIR}/twptest` 
else
    echo "Engine not found"
    exit 1
fi


pushd ${WORKDIR}
export TWP_CONTENT_PATH=testcontents-1
mkdir testcontents-1
echo "start to run test cases ..."
${WORKDIR}/twptest
echo "end of test cases ..."
echo "cleanup test contents ..."
rm -rf testcontents-1
popd
