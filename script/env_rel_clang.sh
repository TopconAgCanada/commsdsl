#!/bin/bash


SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"
ROOT_DIR=$( dirname ${SCRIPT_DIR} )
BUILD_DIR="${ROOT_DIR}/build.clang.rel"
mkdir -p ${BUILD_DIR}
cd ${BUILD_DIR}

CC=clang CXX=clang++ cmake .. -DCMAKE_INSTALL_PREFIX=install -DCMAKE_BUILD_TYPE=Release -DCOMMSDSL_BUILD_UNIT_TESTS=ON "$@"
