.POSIX:

XCFLAGS = $(CFLAGS) -Wall -Wextra -Wshadow -pedantic -D_XOPEN_SOURCE=700 -Og -g

OBJS = utils.o list.o search.o

all: kiss

kiss: main.o libkiss.a
	$(CC) $(LDFLAGS) main.o libkiss.a -o $@

libkiss.a: $(OBJS)
	$(AR) -rcs $@ $(OBJS)

.c.o:
	$(CC) $(XCFLAGS) -c $< -o $@

test: kiss-test
	./kiss-test

kiss-test: test.o libkiss.a
	$(CC) $(LDFLAGS) test.o libkiss.a -o $@

clean:
	rm -f libkiss.a test.o test $(OBJS) main.o kiss kiss-test

.PHONY: clean test all

utils.o: kiss.h
test.o: kiss.h
list.o: kiss.h
search.o: kiss.h
main.o: kiss.h
