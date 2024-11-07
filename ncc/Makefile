CFLAGS=-std=c11 -g -static
SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)

NC_FILES:=$(wildcard ./toAssert/*.nc)

ncc: $(OBJS)
	$(CC) -o ncc $(OBJS) $(LDFLAGS)

assert: ncc
	@for file in $(NC_FILES); do \
        echo "Processing $$file\n"; \
		./ncc $$file > tmp.s; \
		gcc -o tmp tmp.s -z execstack; \
		cat $$file; \
		echo "\n";\
		./tmp; \
		echo "\nreturn value: $$?\n\n";\
	done

tmp: tmp.s
	gcc -o tmp tmp.s -z execstack
	./tmp ; echo "$$?"

clean:
	rm -f ncc *.o *~ tmp*

.PHONY: clean assert