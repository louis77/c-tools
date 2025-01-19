tests: tests.o
	$(CC) -o tests tests.o

tests.o: tests.c
	$(CC) -c tests.c

test: tests
	./tests