#!/bin/bash

if [[ $UID -ne 0 ]]; then
  echo "Superuser privileges are required to run this script."
  echo "Use: \"sudo $0\""
  exit 1
fi

# check system info
arch=$(uname -m)
os=$(cat /etc/lsb-release | grep -i 'DISTRIB_ID' | awk -F '=' '{print $2}')
osVersion=$(cat /etc/lsb-release | grep -i 'Release' | awk -F '=' '{print $2}')

if [ ${arch} = 'aarch64' ]; then
  echo "Preparing necessary dependencies"
  if [[ ${os} = 'Ubuntu' ]]; then
    # clear apt lock
    rm -f /var/lib/dpkg/lock
    rm -f /var/cache/apt/archives/lock
    dpkg --configure -a

    apt update
    apt install -y gcc
    apt install -y g++
    apt install -y make
  elif [[ ${os} = 'Kylin' ]]; then
    yum install -y gcc-c++
    yum install -y protobuf-compiler
  fi
else
  echo "Your system is ""${arch}"
fi
