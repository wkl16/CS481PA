#!/bin/bash

IN_FILE=$1
OUT_FILE=$(echo $IN_FILE | sed 's/.txt/.csv/')

echo "$IN_FILE -> $OUT_FILE"

echo "step,arrive,reject,wait" > "$OUT_FILE"
awk '/^ *[0-9]+/ {print $1 "," $3, "," $5 "," $7}' "$IN_FILE" >> "$OUT_FILE"
