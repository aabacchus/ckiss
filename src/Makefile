.POSIX:

XCFLAGS = $(CFLAGS) -Wall -Wextra -Wshadow -pedantic -D_XOPEN_SOURCE=700 -Og -g

OBJS = utils.o list.o search.o array.o checksum.o checksums.o pkg.o update.o
LIBS = -lblake3

all: ckiss

ckiss: main.o libckiss.a
	$(CC) -o $@ $(LDFLAGS) main.o libckiss.a $(LIBS)

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

utils.o: array.h utils.h
test.o: utils.h
list.o: actions.h utils.h
search.o: actions.h array.h utils.h
array.o: array.h utils.h
checksum.o: actions.h pkg.h utils.h checksums.h
checksums.o: utils.h checksums.h
pkg.o: pkg.h utils.h
update.o: actions.h utils.h
main.o: actions.h utils.h
