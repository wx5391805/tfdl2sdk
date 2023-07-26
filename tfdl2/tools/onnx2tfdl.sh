#!/usr/bin/env bash
ROOT_PATH=""
if [ -z $TFDL_SDK_PATH ];then
	ROOT_PATH="./"
fi

export LD_LIBRARY_PATH=./lib/:$TFDL_SDK_PATH/lib/:$LD_LIBRARY_PATH
#!/usr/bin/env bash

ONNXMODEL=
OUTPUT=
CALIBRATION_MODE=0
CALIBRATION_SET=
CALIBRATION_TYPE="img"
CALIBRATION_LOADER=
BINARY_SET=
TXT_FILE=
INPUT_SHAPE=
SAME_CONFIG=0
COVERAGE=0.995
SYMMETRICAL=0
DEBUG=0
RGB=0
FRUGAL=0
CALIBRATE_WEIGHT=0
QUANT_CALIBRATE=0
EXPECTANT_COS=-1000000000
WEIGHT_TUNING=0
RANDOM_INPUT=
LOAD_PARAMETER=
DUMP_PARAMETER=
CI=false
HEIGHT=0
WIDTH=0
MEAN_VALUE="[]"
SCALE="[]"
PADDING_STRATEGY="extra_padding"
QUANTIZE_LAYER_TYPE=

function usage() {
    echo "Usage:"

    echo "-h, --help                       print help message"
    echo "-p, --onnxmodel <args>           input onnx mode file path"
    echo "-o, --output <args>              output model name"
    echo "-i, --height <args>              onnx model preset input height"
    echo "-j, --width <args>               onnx model preset input width, height and width must be given at the same time"
    echo "-m, --mean_value <args>          mean value of preprocess"
    echo "-a, --scale <args>               scale of preprocess"
    echo "-g, --padding_strategy <args>    choose auto padding upper handle strategy, \"extra_padding\" or \"flip_conv\""
    echo "-c, --calibration_mode <args>    choose which calibration model to use"
    echo "-s, --calibration_set <args>     calibration data set, if binary_set and this are both not implemented, use calibration.bin"
    echo "-t, --calibration_type <args>    calibration data type, img and npy supported, default to be img"
    echo "-l, --calibration_loader <args>  calibration min/max value loader file"
    echo "-b, --binary_set <args>          binary input data set, if calibration_set and this are both not implementer, use calibration.bin"
    echo "-T, --txt_file <args>            txt input data"
    echo "-S, --input_shape <args>         input's shape, should be like 1x2x3x4 or 1*2*3*4. Multiple inputs are separated by \',\'. like \"1x2x3x4,4x3x2x1\""
    echo "-f, --same_config <args>         whether to keep same config in a eltwise layer, default to be true"
    echo "-v, --coverage <args>            coverage value, if not implemented, use 0.9995"
    echo "-y, --symmetrical <args>         whether to use symmetrical mode in MES calibration"
    echo "-d, --debug <args>               whether to print debug information after calibration"
    echo "-r, --rgb <args>                 whether to use RGB instead of BGR for input images"
    echo "-u, --frugal <args>              whether to enable frugal inference mode"
    echo "-w, --calibrate_weight <args>    choose the level of weight calibration"
    echo "-q, --quant_calibrate <args>     calibrate mid-value using inputs after fake quantization layer by layer"
    echo "-e, --expectant_cos <args>       optimize the network until the cosine similarity reaches the desired value. (in [-1, 1])"
    echo "-n, --weight_tuning <args>       tuning weight and bias of convolution to achieve minimum cosine distance"
    echo "-R, --random_input <args>        Implement a number and converter will use this number as calibration size and inference with gauss random inputs"
    echo "-L, --quantize_layer_type <args> Specifying layer types that need to be quantized. If not implemented, all layers will be quantized"
    echo "-I, --load_parameter <args>      load calibration parameters directly from json file"
    echo "-O, --dump_parameter <args>      dump calibration parameters to certain json file"

    exit 0
}

function parseOptions() {
    ARGS=`getopt -o hp:i:j:m:a:g:o:c:s:t:l:b:T:S:f:v:y:d:r:u:w:q:e:n:R:L:I:O: -l help,onnxmodel:,height:,width:,mean_value:,scale:,padding_strategy:,output:,calibration_mode:,calibration_set:,calibration_type:,calibration_loader:,binary_set:,txt_file:,input_shape:,same_config:,coverage:,symmetrical:,debug:,rgb:,frugal:,calibrate_weight:,CI:,quant_calibrate:,expectant_cos:,weight_tuning:,random_input:,quantize_layer_type:,load_parameter:,dump_parameter: -n 'newConverter' -- "$@"`
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
        -p|--onnxmodel)
            ONNXMODEL=$2
            shift 2
            ;;
        -i|--height)
            HEIGHT=$2
            shift 2
            ;;
        -j|--width)
            WIDTH=$2
            shift 2
            ;;
        -m|--mean_value)
            MEAN_VALUE=$2
            shift 2
            ;;
        -a|--scale)
            SCALE=$2
            shift 2
            ;;
        -g|--padding_strategy)
            PADDING_STRATEGY=$2
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
	    -T|--txt_file)
	        TXT_FILE=$2
	        shift 2
	        ;;
	      -S|--input_shape)
            INPUT_SHAPE=$2
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
        -q|--quant_calibrate)
            QUANT_CALIBRATE=$2
            shift 2
            ;;
        --CI)
            CI=$2
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
        -R|--random_input)
            RANDOM_INPUT=$2
            shift 2
            ;;
        -L|--quantize_layer_type)
            QUANTIZE_LAYER_TYPE=$2
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

${ROOT_PATH}onnx2tfdl --onnx "${ONNXMODEL}" --output "${OUTPUT}" --input_height "${HEIGHT}" --input_width "${WIDTH}" --input_shape "${INPUT_SHAPE}" --scale "${SCALE}" --mean_value "${MEAN_VALUE}" --padding_strategy "${PADDING_STRATEGY}"

if [[ "${CI}" != "true" ]]; then
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
     --binary_set "${BINARY_SET}" --txt_file "${TXT_FILE}" --output "${OUTPUT}_int8.bin" --same_config "${SAME_CONFIG}" --coverage "${COVERAGE}" \
     --symmetrical "${SYMMETRICAL}" --debug "${DEBUG}" --rgb "${RGB}" --frugal "${FRUGAL}" --calibrate_weight "${CALIBRATE_WEIGHT}" \
     --quant_calibrate "${QUANT_CALIBRATE}" --expectant_cos "${EXPECTANT_COS}" --weight_tuning "${WEIGHT_TUNING}" \
     --random_input "${RANDOM_INPUT}" --quantize_layer_type "${QUANTIZE_LAYER_TYPE}" --load_parameter "${LOAD_PARAMETER}" --dump_parameter "${DUMP_PARAMETER}"
  #fi
fi
