#!/bin/bash

gcc -Wall -g -o broken_list broken_list.c
if [ $? -ne 0 ]; then
    echo "Build Test Failed!"
    exit 1
fi

OUTPUT=$(./broken_list)
echo "$OUTPUT"

echo "$OUTPUT" | grep -q "Found Alice!"
if [ $? -ne 0 ]; then
    echo "Logic Test Failed Alice"
    exit 1
fi

echo "$OUTPUT" | grep -q "Charlie removed successfully."
if [ $? -ne 0 ]; then
    echo "Logic Test Failed Charlie"
    exit 1
fi

valgrind --error-exitcode=1 --leak-check=full ./broken_list
if [ $? -ne 0 ]; then
    echo "Memory Leak Detected!"
    exit 1
fi

echo "ALL TESTS PASSED"
