.POSIX:

XCFLAGS = $(CFLAGS) -Wall -Wextra -Wshadow -pedantic -D_XOPEN_SOURCE=700 -Og -g

OBJS = main.o list.o

all: kiss

kiss: $(OBJS) libkiss.a
	$(CC) $(LDFLAGS) $(OBJS) libkiss.a -o $@

libkiss.a: libkiss.o
	$(AR) -rcs $@ libkiss.o

.c.o:
	$(CC) $(XCFLAGS) -c $< -o $@

test: kiss-test
	./kiss-test

kiss-test: test.o libkiss.a
	$(CC) $(LDFLAGS) test.o libkiss.a -o $@

clean:
	rm -f libkiss.a libkiss.o test.o test $(OBJS) kiss kiss-test

.PHONY: clean test all

libkiss.o: kiss.h
test.o: kiss.h
list.o: kiss.h
main.o: kiss.h
