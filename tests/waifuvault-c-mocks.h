// Mocks for unit testing
#ifndef WAIFUVAULT_C_MOCKS_H
#define WAIFUVAULT_C_MOCKS_H
#include <stdio.h>
#include<curl/curl.h>
#include "../waifuvault-c-models.h"

struct dispatchMock {
    int calls;
    CURLcode returns;
    MemoryStream *contents;
    char *targetUrl;
    char *targetMethod;
    char *fields;
    struct curl_httppost *formpost;
    struct curl_slist *headers;
};

extern struct dispatchMock dispatchMock;

#endif