CFLAGS=-std=c11 -g -static

ncc: ncc.c

assert: ncc
	bash assert.sh

clean:
	rm -f ncc *.o *~ tmp*

.PHONY: assert clean