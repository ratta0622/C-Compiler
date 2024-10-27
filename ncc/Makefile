CFLAGS=-std=c11 -g -static

ncc: ncc.c

assert: ncc
	bash assert.sh

tmp: tmp
	gcc -o tmp tmp.s -z execstack
	./tmp ; echo "$$?"

test: test.txt
	bash test.sh

clean:
	rm -f ncc *.o *~ tmp* test.txt

.PHONY: assert clean