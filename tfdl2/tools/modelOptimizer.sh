#!/usr/bin/env bash
ROOT_PATH=""
if [ -z $TFDL_SDK_PATH ];then
	ROOT_PATH="./"
fi

export LD_LIBRARY_PATH=./lib/:$TFDL_SDK_PATH/lib/:$LD_LIBRARY_PATH
${ROOT_PATH}tfdl_modelOptimizer $@
#${ROOT_PATH}tfdl_modelOptimizer /path/to/json /path/to/bin output_json_name output_bin_name