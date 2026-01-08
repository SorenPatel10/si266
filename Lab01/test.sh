#!/bin/bash

make -s clean
make -s all

EXPECTED=">> System Ready for Lab 1"
OUTPUT=$(./calc)

if [ "$OUTPUT" == "$EXPECTED" ]; then
    echo -e "\033[0;32mPASS\033[0m"
    exit 0
else
    echo -e "\033[0;31mFAIL\033[0m"
    exit 1
fi