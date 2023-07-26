#!/usr/bin/env bash
OS_ID=`cat /etc/lsb-release | grep -i 'Id' | awk -F '=' '{print $2}'`
OS_VER=`cat /etc/lsb-release | grep -i 'Release' | awk -F '=' '{print $2}'`
if [ ${OS_VER} = '16.04' ]
then
	export LD_LIBRARY_PATH=./lib/1604/:$LD_LIBRARY_PATH
else
	export QT_PLUGIN_PATH=./lib/1804/
	export LD_LIBRARY_PATH=./lib/1804/:$LD_LIBRARY_PATH
fi
export LD_LIBRARY_PATH=./lib/:$LD_LIBRARY_PATH
./TFDL_Simulator
