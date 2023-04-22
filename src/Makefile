.POSIX:

XCFLAGS = $(CFLAGS) -Wall -Wextra -Wshadow -pedantic -D_XOPEN_SOURCE=700 -Og -g

OBJS = utils.o list.o search.o

all: ckiss

ckiss: main.o libckiss.a
	$(CC) $(LDFLAGS) main.o libckiss.a -o $@

libckiss.a: $(OBJS)
	$(AR) -rcs $@ $(OBJS)

.c.o:
	$(CC) $(XCFLAGS) -c $< -o $@

test: ckiss-test
	./ckiss-test

ckiss-test: test.o libckiss.a
	$(CC) $(LDFLAGS) test.o libckiss.a -o $@

clean:
	rm -f libckiss.a test.o test $(OBJS) main.o ckiss ckiss-test

.PHONY: clean test all

utils.o: ckiss.h
test.o: ckiss.h
list.o: ckiss.h
search.o: ckiss.h
main.o: ckiss.h
