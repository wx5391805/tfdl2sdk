#!/usr/bin/env bash
ROOT_PATH=""
if [ -z $TFDL_SDK_PATH ];then
	ROOT_PATH="./"
fi

export LD_LIBRARY_PATH=./lib/:$TFDL_SDK_PATH/lib/:$LD_LIBRARY_PATH
#!/usr/bin/env bash

PROTOTXT=
CAFFEMODEL=
OUTPUT=
CALIBRATION_MODE=0
CALIBRATION_SET=
CALIBRATION_TYPE="img"
CALIBRATION_LOADER=
BINARY_SET=
SAME_CONFIG=0
COVERAGE=0.995
SYMMETRICAL=0
DEBUG=0
RGB=0
FRUGAL=0
CALIBRATE_WEIGHT=0
QUANT_CALIBRATE=0
PREPROCESS_CONFIG=
EXPECTANT_COS=-1000000000
WEIGHT_TUNING=0
LOAD_PARAMETER=
DUMP_PARAMETER=

function usage() {
    echo "Usage:"

    echo "-h, --help                       print help message"
    echo "-p, --prototxt <args>            input prototxt file path"
    echo "-m, --caffemodel <args>          input caffemodel file path"
    echo "-o, --output <args>              output model name"
    echo "-c, --calibration_mode <args>    choose which calibration model to use"
    echo "-s, --calibration_set <args>     calibration set, if binary_set and this are both not implemented, use calibration.bin"
    echo "-t, --calibration_type <args>    calibration data type, img and npy supported, default to be img"
    echo "-l, --calibration_loader <args>  calibration min/max value loader file"
    echo "-b, --binary_set <args>          binary input data set, if calibration_set and this are both not implementer, use calibration.bin"
    echo "-f, --same_config <args>         whether to keep same config in a eltwise layer, default to be true"
    echo "-v, --coverage <args>            coverage value, if not implemented, use 0.9995"
    echo "-y, --symmetrical <args>         whether to use symmetrical mode in MES calibration"
    echo "-d, --debug <args>               whether to print debug information after calibration"
    echo "-r, --rgb <args>                 whether to use RGB instead of BGR for input images"
    echo "-u, --frugal <args>              whether to enable frugal inference mode"
    echo "-w, --calibrate_weight <args>    choose the level of weight calibration"
    echo "-g, --preprocess_config <args>   preprocess_config, if preprocess_config is implemented, add ImagePreprocessLayer"
    echo "-q, --quant_calibrate <args>     calibrate mid-value using inputs after fake quantization layer by layer"
    echo "-e, --expectant_cos <args>       optimize the network until the cosine similarity reaches the desired value. (in [-1, 1])"
    echo "-n, --weight_tuning <args>       tuning weight and bias of convolution to achieve minimum cosine distance"
    echo "-I, --load_parameter <args>      load calibration parameters directly from json file"
    echo "-O, --dump_parameter <args>      dump calibration parameters to certain json file"
    exit 0
}

function parseOptions() {
    ARGS=`getopt -o hp:m:o:c:s:t:l:b:f:v:y:d:r:u:w:g:q:e:n:I:O: -l help,prototxt:,caffemodel:,output:,calibration_mode:,calibration_set:,calibration_type:,calibration_loader:,binary_set:,same_config:,coverage:,symmetrical:,debug:,rgb:,frugal:,calibrate_weight:,preprocess_config:,quant_calibrate:,expectant_cos:,weight_tuning:,load_parameter:,dump_parameter: -n 'newConverter' -- "$@"`
    if [ $? != 0 ]
    then
        echo "Get option failed."
        usage
        exit 1
    fi

    eval set -- "${ARGS}"

    while true
    do
        case "$1" in
        -h|--help)
            usage
            shift
            ;;
        -p|--prototxt)
            PROTOTXT=$2
            shift 2
            ;;
        -m|--caffemodel)
            CAFFEMODEL=$2
            shift 2
            ;;
        -o|--output)
            OUTPUT=$2
            shift 2
            ;;
        -c|--calibration_mode)
            CALIBRATION_MODE=$2
            shift 2
            ;;
        -s|--calibration_set)
            CALIBRATION_SET=$2
            shift 2
            ;;
        -t|--calibration_type)
            CALIBRATION_TYPE=$2
            shift 2
            ;;
        -l|--calibration_loader)
            CALIBRATION_LOADER=$2
            shift 2
            ;;
        -b|--binary_set)
            BINARY_SET=$2
            shift 2
            ;;
        -f|--same_config)
            SAME_CONFIG=$2
            shift 2
            ;;
        -v|--coverage)
            COVERAGE=$2
            shift 2
            ;;
        -y|--symmetrical)
            SYMMETRICAL=$2
            shift 2
            ;;
        -d|--debug)
            DEBUG=$2;
            shift 2
            ;;
        -r|--rgb)
            RGB=$2;
            shift 2
            ;;
        -u|--frugal)
            FRUGAL=$2;
            shift 2
            ;;
        -w|--calibrate_weight)
            CALIBRATE_WEIGHT=$2
            shift 2
            ;;
        -g|--preprocess_config)
            PREPROCESS_CONFIG=$2
            shift 2
            ;;
        -q|--quant_calibrate)
            QUANT_CALIBRATE=$2
            shift 2
            ;;
        -e|--expectant_cos)
            EXPECTANT_COS=$2
            shift 2
            ;;
        -n|--weight_tuning)
            WEIGHT_TUNING=$2
            shift 2
            ;;
        -I|--load_parameter)
            LOAD_PARAMETER=$2
            shift 2
            ;;
        -O|--dump_parameter)
            DUMP_PARAMETER=$2
            shift 2
            ;;
        --)
            shift
            break
            ;;
        *)
            echo "Parsing error!"
            exit 1
            ;;
        esac
    done
}

if [ ! -n "$1" ] ;then
    echo "No input argument detected, you can use <-h|--help> to get help message"
    exit 1
fi

parseOptions $*

if [ ! -f "$CAFFEMODEL" ]; then
    ${ROOT_PATH}caffe2tfdl "${PROTOTXT}" "${OUTPUT}"
else
    ${ROOT_PATH}caffe2tfdl "${PROTOTXT}" "${CAFFEMODEL}" "${OUTPUT}"
fi

if [ -f "$PREPROCESS_CONFIG" ]; then
    ${ROOT_PATH}tfdl_modelPreprocess "${OUTPUT}.json" "${OUTPUT}_fp32.bin" "${PREPROCESS_CONFIG}" "${OUTPUT}_tmp.json" "${OUTPUT}_tmp_fp32.bin"
    mv "${OUTPUT}_tmp.json" "${OUTPUT}.json"
    mv "${OUTPUT}_tmp_fp32.bin" "${OUTPUT}_fp32.bin"
fi

${ROOT_PATH}tfdl_modelOptimizer "${OUTPUT}.json" "${OUTPUT}_fp32.bin" "${OUTPUT}.json" "${OUTPUT}_fp32.bin"
echo "quantizing..."
#if [[ "${FRUGAL}" == "1" ]]; then
#  if [[ -z "${CALIBRATION_SET}" ]]; then
#    ${ROOT_PATH}tfdl_modelConverter "${OUTPUT}.json" "${OUTPUT}_fp32.bin" "${OUTPUT}_int8.bin"
#  else
#    ${ROOT_PATH}tfdl_modelConverter "${OUTPUT}.json" "${OUTPUT}_fp32.bin" "${CALIBRATION_SET}" "${OUTPUT}_int8.bin"
#  fi
#else
  ${ROOT_PATH}tfdl.quantizer --json "${OUTPUT}.json" --model "${OUTPUT}_fp32.bin" --calibration_mode "${CALIBRATION_MODE}" \
   --calibration_set "${CALIBRATION_SET}" --calibration_type "${CALIBRATION_TYPE}" --calibration_loader "${CALIBRATION_LOADER}" \
   --binary_set "${BINARY_SET}" --output "${OUTPUT}_int8.bin" --same_config "${SAME_CONFIG}" --coverage "${COVERAGE}" \
   --symmetrical "${SYMMETRICAL}" --debug "${DEBUG}" --rgb "${RGB}" --frugal "${FRUGAL}" --calibrate_weight "${CALIBRATE_WEIGHT}" \
   --quant_calibrate "${QUANT_CALIBRATE}" --expectant_cos "${EXPECTANT_COS}" --weight_tuning "${WEIGHT_TUNING}" \
   --load_parameter "${LOAD_PARAMETER}" --dump_parameter "${DUMP_PARAMETER}"
#fi
