#!/bin/bash

make -s clean
make -s all

valgrind --leak-check=full --error-exitcode=1 ./parser > program.out
VALGRIND_EXITCODE=$?
EXPECTED="Sensor 0: Avg Temp = 66.23"
OUTPUT=$(grep -m 1 "" program.out)


if [ "$OUTPUT" != "$EXPECTED" ]; then
    echo "Logic Check FAILED"
    exit 1
fi

if [ "$VALGRIND_EXITCODE" -ne 0 ]; then
    echo "Memory Check FAILED"
    exit 1
fi

echo "All Tests Passed"
exit 0

