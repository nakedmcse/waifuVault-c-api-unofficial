all: sdk tests

sdk: waifuvault-c-api.c waifuvault-c-api.h waifuvault-c-models.h waifuvault-c-utils.h waifuvault-c-deserializers.h mjson.c mjson.h
	gcc -O3 -w -c mjson.c -c waifuvault-c-api.c

tests: mjson.o waifuvault-c-api.o tests/waifuvault-c-test.c tests/waifuvault-c-buckets-test.c tests/waifuvault-c-restrictions-test.c
	gcc -O3 -w -o tests/waifuvault-c-test tests/waifuvault-c-test.c mjson.o waifuvault-c-api.o -lcurl
	gcc -O3 -w -o tests/waifuvault-c-buckets-test tests/waifuvault-c-buckets-test.c mjson.o waifuvault-c-api.o -lcurl
	gcc -O3 -w -o tests/waifuvault-c-restrictions-test tests/waifuvault-c-restrictions-test.c mjson.o waifuvault-c-api.o -lcurl
	gcc -O3 -w -o tests/waifuvault-c-deserializer-test tests/waifuvault-c-deserializer-test.c mjson.o waifuvault-c-api.o -lcurl

clean:
	rm -f *.o
	rm -f tests/waifuvault-c-test
	rm -f tests/waifuvault-c-buckets-test
	rm -f tests/waifuvault-c-restrictions-test