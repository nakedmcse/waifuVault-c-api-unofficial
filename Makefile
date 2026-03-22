all: sdk tests

alltests: sdktests unittests

sdk: waifuvault-c-api.c waifuvault-c-api.h waifuvault-c-models.h waifuvault-c-utils.h waifuvault-c-deserializers.h cJSON.c cJSON.h
	gcc -O3 -w -c cJSON.c -c waifuvault-c-api.c

sdktests: waifuvault-c-api.c waifuvault-c-api.h waifuvault-c-models.h waifuvault-c-utils.h waifuvault-c-deserializers.h cJSON.c cJSON.h
	gcc -DWAIFUVAULT_C_UNIT_TEST -O3 -w -c cJSON.c -c waifuvault-c-api.c

tests: cJSON.o cJSON.h waifuvault-c-api.h waifuvault-c-models.h waifuvault-c-api.o tests/waifuvault-c-test.c tests/waifuvault-c-buckets-test.c tests/waifuvault-c-restrictions-test.c tests/waifuvault-c-albums-test.c
	gcc -O3 -w -o tests/waifuvault-c-test tests/waifuvault-c-test.c cJSON.o waifuvault-c-api.o -lcurl
	gcc -O3 -w -o tests/waifuvault-c-buckets-test tests/waifuvault-c-buckets-test.c cJSON.o waifuvault-c-api.o -lcurl
	gcc -O3 -w -o tests/waifuvault-c-restrictions-test tests/waifuvault-c-restrictions-test.c cJSON.o waifuvault-c-api.o -lcurl
	gcc -O3 -w -o tests/waifuvault-c-albums-test tests/waifuvault-c-albums-test.c cJSON.o waifuvault-c-api.o -lcurl

unittests: cJSON.o cJSON.h waifuvault-c-api.h waifuvault-c-models.h waifuvault-c-api.o tests/waifuvault-c-unittests.c
	gcc -O3 -w -o tests/waifuvault-c-unittests tests/waifuvault-c-unittests.c cJSON.o waifuvault-c-api.o -lcurl

clean:
	rm -f *.o
	rm -f tests/waifuvault-c-test
	rm -f tests/waifuvault-c-buckets-test
	rm -f tests/waifuvault-c-restrictions-test
	rm -f tests/waifuvault-c-deserializer-test
	rm -f tests/waifuvault-c-albums-test
	rm -f tests/waifuvault-c-unittests
