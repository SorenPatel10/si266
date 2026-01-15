#!/bin/bash

make -s clean
make -s all

valgrind --leak-check=full --error-exitcode=1 ./part2 > program.out
VALGRIND_EXITCODE=$?
EXPECTED="[42] Hopper, Grace (GPA: 5.00)"
OUTPUT=$(cat program.out)


if [ "$OUTPUT" == "$EXPECTED" ]; then
    echo "Info Check PASSED"
    INFO_CHECK=1
else
    echo "Info Check FAILED"
fi

if [ "$VALGRIND_EXITCODE" -eq 0 ]; then
    echo "Memory Check PASSED"
    MEM_CHECK=1
else
    echo "Memory Check FAILED"
fi

if [ $INFO_CHECK -eq 1 ] && [ $MEM_CHECK -eq 1 ]; then
    exit 0
else
    exit 1
fi

