#!/bin/sh

THIS_PATH="$(realpath "$0")"
THIS_DIR="$(dirname "$THIS_PATH")"

FILE_LIST="$(find "$THIS_DIR" | grep -E ".*(\.cc|\.c|\.h|\.inl)$")"

echo -e "Files found to format = \n\"\"\"\n$FILE_LIST\n\"\"\""

clang-format --verbose -i --style=file $FILE_LIST
