#!/bin/sh

for n in $(seq 2 2 7); do
    for m in $(seq 7 2 10); do
        FILE="${n}_${m}.txt"
        echo "running ./ride -N $n -M $m > $FILE"
        ./ride -N $n -M $m > $FILE
        ./csv.sh $FILE
    done
done
