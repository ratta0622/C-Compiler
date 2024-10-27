CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)

ncc: $(OBJS)
	$(CC) -o ncc $(OBJS) $(LDFLAGS)

assert: ncc
	bash assert.sh

tmp: tmp.s
	gcc -o tmp tmp.s -z execstack
	./tmp ; echo "$$?"

test:
	bash test.sh

clean:
	rm -f ncc *.o *~ tmp*

.PHONY: assert clean test