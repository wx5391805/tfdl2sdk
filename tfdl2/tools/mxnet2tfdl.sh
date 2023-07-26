#!/usr/bin/env bash
ROOT_PATH=""
if [ -z $TFDL_SDK_PATH ];then
	ROOT_PATH="./"
fi

export LD_LIBRARY_PATH=./lib/:$TFDL_SDK_PATH/lib/:$LD_LIBRARY_PATH

${ROOT_PATH}mxnet2tfdl $1 $2 $3.json $3_fp32.bin

if [ $? != "0" ]; then
	exit 1;
fi

${ROOT_PATH}tfdl_modelOptimizer $3.json $3_fp32.bin $3.json $3_fp32.bin

if [ $? != "0" ]; then
	exit 2;
fi

echo "quantizing..."
if [ -z $4 ];then
	${ROOT_PATH}tfdl_modelConverter $3.json $3_fp32.bin $3_int8.bin
else
	${ROOT_PATH}tfdl_modelConverter $3.json $3_fp32.bin $4 $3_int8.bin
fi

if [ $? != "0" ]; then
	exit 3;
fi