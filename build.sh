#!/usr/bin/env bash

set -eu

mkdir -p obj
mkdir -p bin

LINK_FLAGS=""
COMPILER_FLAGS="-g -Wall"
IFLAGS=$(find . -name "*.h" | xargs dirname | uniq | awk '{ print "-I" $0 }')

SRC_FILES=$(find . -name "*.c")

for FILE in $SRC_FILES; do
  OBJ_PATH="./obj/$(dirname $FILE | sed 's/\.\///')/$(basename $FILE | sed 's/\.c/\.o/')";
  mkdir -p $(dirname $OBJ_PATH)
  echo "gcc $COMPILER_FLAGS $IFLAGS $FILE -o $OBJ_PATH -c";
  gcc $COMPILER_FLAGS $IFLAGS $FILE -o $OBJ_PATH -c;
done

OBJ_FILES=$(find obj -name "*.o")

echo "gcc -o bin/main $LINK_FLAGS $OBJ_FILES"
gcc -o bin/main $LINK_FLAGS $OBJ_FILES
