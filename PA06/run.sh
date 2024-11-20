#!/bin/sh

TABLE_FILE="table.txt"
echo n,m,arrived,rejected,ratio,wait_time,max_line > $TABLE_FILE
for n in $(seq 2 2 7); do
    for m in $(seq 7 2 10); do
        FILE="${n}_${m}.txt"
        echo "running ./ride -N $n -M $m > $FILE"
        ./ride -N $n -M $m > $FILE
        ./csv.sh $FILE

        ARRIVED=$(grep 'arrived' "$FILE" | awk '{print $3}')
        REJECTED=$(grep 'rejected' "$FILE" | awk '{print $3}')
        RATIO=$(awk "BEGIN {x=${ARRIVED}; y=${REJECTED}; printf \"%.2f\", (y/x*100)}")
        WAIT_TIME=$(grep 'wait time' "$FILE" | awk '{print $4}')
        MAX_LINE=$(grep 'wait line' "$FILE" | awk '{print $4,$5,$6,$7}')

        echo $n, $m, $ARRIVED, $REJECTED, $RATIO, $WAIT_TIME, $MAX_LINE >> $TABLE_FILE
    done
done
