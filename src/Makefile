.POSIX:

XCFLAGS = $(CFLAGS) -Wall -Wextra -pedantic -D_XOPEN_SOURCE=700 -Og -g

libkiss.a: libkiss.o
	$(AR) -rcs $@ libkiss.o

.c.o:
	$(CC) $(XCFLAGS) -c $< -o $@

test: test.o libkiss.a
	$(CC) $(LDFLAGS) test.o libkiss.a -o $@

clean:
	rm -f libkiss.a libkiss.o test.o test

.PHONY: clean

libkiss.o: kiss.h
test.o: kiss.h
