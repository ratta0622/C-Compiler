#!/bin/bash
assert(){
    expected="$1"
    input="$2"

    ./ncc "$input" > tmp.s
    cc -o tmp tmp.s -z noexecstack
    ./tmp
    actual="$?"

    if [ "$actual" = "$expected" ]; then
        echo "$input -> $actual"
    else
        echo "$input -> $expected expected, but got $actual"
        exit 1
    fi
}

for i in {0..10}; do
    a=$((RANDOM%10))
    b=$((RANDOM%10))
    c=$((RANDOM%10))
    d=$((RANDOM%10))
    ans=$((a+b-c+d))
    assert "$ans" "$a+$b-$c+$d"
done

echo OK