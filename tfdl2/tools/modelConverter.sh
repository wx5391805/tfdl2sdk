#!/usr/bin/env bash
ROOT_PATH=""
if [ -z $TFDL_SDK_PATH ];then
	ROOT_PATH="./"
fi

export LD_LIBRARY_PATH=./lib/:$TFDL_SDK_PATH/lib/:$LD_LIBRARY_PATH
${ROOT_PATH}tfdl_modelConverter $@
#${ROOT_PATH}tfdl_modelConverter /path/to/json /path/to/bin /path/to/imagelist output_name