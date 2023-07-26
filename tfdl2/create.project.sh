#!/bin/bash


############################################################################################
# variables

TFDL_SDK_PATH=$(dirname $(readlink -f ${BASH_SOURCE[0]}))

PROJECT_TYPES=("basic" "classification")

OPT_NAME=
OPT_TYPE=
OPT_OVERWRITE=0

############################################################################################
# methods

# 
function print_usage()
{
  echo 
  echo "Usage:"
  echo "./create.project.sh [OPTION]"
  echo

  echo "Options:"
  echo " -h, --help                   print help message"
  echo " -p, --project <project name> specify project name"
  echo " -t, --type <project type>    specify project type, can be basic/classification"
  echo " -w, --overwrite              overwrite if project with the same name exists"
  echo
}

# 
function parse_options()
{ 
  ARGS=`getopt -o hwp:t: -l help,overwrite,project:,type:, -n 'create.project' -- "$@"`
  if [ $? != 0 ]
  then
    echo "Get option failed."
    print_usage
    exit 1
  fi
  
  eval set -- "${ARGS}"
  
  while true
  do
    case "$1" in
    -h|--help) 
      print_usage
      exit 0
      ;;
    -w|--overwrite) 
      OPT_OVERWRITE=1
      shift
      ;;
    -p|--project)
      OPT_NAME=$2
      shift 2
      ;;      
    -t|--type)
      OPT_TYPE=$2
      shift 2
      ;;
    --)
      shift
      break
      ;;
    *)
      echo "Quit: Parsing failed"
      exit 1
      ;;
    esac
  done
}

# 
function check_options()
{
  echo
  echo "[1] Check options:"
  
  if [ -z ${OPT_NAME} ] 
  then
    read -r -p "Project name not specified, generate random? [Y/N] " choice
    if [ "${choice}" != "y" -a "${choice}" != "Y" ]
    then
      read -r -p "Input name: " choice
      if [ -n "${choice}" ]
      then
        OPT_NAME=${choice}
      else
        echo "Quit: no project name specified."
        exit -1
      fi
      else
        OPT_NAME="new".$(head -n 5 /dev/urandom | sed -e 's/[^a-zA-Z0-9]//g' | strings -n 4 | head -1)
        echo "Project name set to ${OPT_NAME}."
    fi
  fi

  project_dir="projects/${OPT_NAME}"
  if [ ! -d "${project_dir}" ]
  then
    mkdir -p "${project_dir}"
  else
    if [ ${OPT_OVERWRITE} -eq 0 ]
    then
      read -r -p "Target project '${OPT_NAME}' exists, overwrite? [Y/N] " choice
    else
      choice='y'
    fi
    
    if [ "${choice}" != "y" -a "${choice}" != "Y" ]
    then
      echo "Quit: avoid overwrite existing project."
      exit -1
    else
      rm -rf "${project_dir}"
      mkdir -p "${project_dir}"
    fi
  fi

  valid_type=0

  [[ "${PROJECT_TYPES[@]/${OPT_TYPE}/}" != "${PROJECT_TYPES[@]}" ]] && valid_type=1

  if [ "${valid_type}" -eq 0 ] 
  then
    echo "Project type not provided or invalid, select from below:"
    n=1
    for i in ${PROJECT_TYPES[@]}
    do 
      echo "$n. $i";
      let n+=1; 
    done
    
    read -r choice
    types=${#PROJECT_TYPES[@]}
    if [ "${choice}" -ge 1 -a "${choice}" -le ${types} ]
    then
      OPT_TYPE=${PROJECT_TYPES[$((choice-1))]}
    else
      OPT_TYPE=${PROJECT_TYPES[0]}
      echo "Invalid index, type is default to '${OPT_TYPE}'"
    fi
    
  fi
  
  echo "+++"
  echo "Create project folder for '${OPT_NAME}', project type: '${OPT_TYPE}'. "
  
  echo "[Done]"
  echo
}

# 
function prepare_project()
{
  echo
  echo "[2] Set up project:"
  
  case "${OPT_TYPE}" in
  basic) 
    ;;
    
  classification)
    dpkg -L libopencv-dev &> /dev/null
    if [ $? -ne 0 ]
    then
      echo "Quit: OpenCV required but not detected on this device."
      echo "Install with: apt-get install libopencv-dev -y"
      exit 0
    fi
    
    ;;

  *)
    ;;
  esac
  
  mkdir -p projects/${OPT_NAME}

  echo "Preparing header files ..."
  cp -Rfp $TFDL_SDK_PATH/include/ projects/${OPT_NAME}/include
    
  echo "Preparing libraries ..."
  cp -Rfp $TFDL_SDK_PATH/lib/ projects/${OPT_NAME}/lib
    
  echo "Preparing project sources ..."
  cp -Rfp $TFDL_SDK_PATH/example/template/${OPT_TYPE}/* projects/${OPT_NAME}/
    
  echo "[Done]"
  echo
}

############################################################################################
# 
parse_options $@

check_options

prepare_project

echo "Creating ${OPT_NAME} Succeed!"
