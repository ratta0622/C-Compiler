#!/bin/bash
rm -f test.txt
touch test.txt

for i in {0..10}; do
    a=$((RANDOM%10))
    b=$((RANDOM%10))
    echo "$a != $b" >> test.txt
    ./ncc "$a!=$b" > tmp.s
    cc -o tmp tmp.s -z noexecstack
    ./tmp
    echo $? >> test.txt
done

