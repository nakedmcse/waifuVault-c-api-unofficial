#!/bin/sh
gcc -o waifuvault-c-test waifuvault-c-test.c ../mjson.o ../waifuvault-c-api.o -lcurl
gcc -o waifuvault-c-buckets-test waifuvault-c-buckets-test.c ../mjson.o ../waifuvault-c-api.o -lcurl
