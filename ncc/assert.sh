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

for i in {0..255}; do
    assert "$i" "$i"
done

echo OK