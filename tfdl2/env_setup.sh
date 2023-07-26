#! /bin/bash

TFDL_SDK_PATH=$(dirname $(readlink -f ${BASH_SOURCE[0]}))
export PATH=$TFDL_SDK_PATH/tools:$PATH
export TFDL_SDK_PATH=$TFDL_SDK_PATH/tools