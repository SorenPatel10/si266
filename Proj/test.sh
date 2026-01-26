#!/bin/bash

OUTPUT=$(./gatherer)

# Regex for number (integer or float)
val="[0-9]+(\.[0-9]+)?"

REGEX="^CPU_USAGE:$val,MEM_USED:$val,MEM_AVAIL:$val,MEM_FREE:$val,MEM_CACHED:$val,SWAP_USED:$val,SWAP_FREE:$val,LOAD_1:$val,LOAD_5:$val,LOAD_15:$val,PROC_RUN:$val,PROC_TOTAL:$val$"

if [[ "$OUTPUT" =~ $REGEX ]]; then
  echo "Valid! All keys present in the correct order."
  exit 0
else
  echo "Invalid! Missing keys or incorrect order."
  echo "$OUTPUT"
  exit 1
fi
