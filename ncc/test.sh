#!/bin/bash
for i in {0..10}; do
    a=$((RANDOM%10))
    b=$((RANDOM%10))
    echo "$a+$b="
    ./ncc "ab=$a; bc =$b; ab+bc;" > tmp.s
    cc -o tmp tmp.s -z noexecstack
    ./tmp
    echo $?
done

