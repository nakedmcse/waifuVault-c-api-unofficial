// Waifuvault C SDK JSON Desrializers
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stddef.h>
#include "waifuvault-c-models.h"
#include "mjson.h"

#ifndef WAIFUVAULT_C_DESERIALIZERS_H
#define WAIFUVAULT_C_DESERIALIZERS_H

FileResponse deserializeResponse(char *body, bool stringRetention) {
    char token[80];
    char bucket[80];
    char url[120];
    char retentionPeriod[80];
    int retentionPeriodInt;
    int jsonStatus = 0;
    bool hasFilename;
    bool oneTimeDownload;
    bool protected;
    char *adjustedBody = (char *)malloc(strlen(body)+100);
    FileResponse retval;
    FileOptions retopts;

    struct json_attr_t options_attrs[] = {
        {"hideFilename", t_boolean, .addr.boolean = &hasFilename},
        {"oneTimeDownload", t_boolean, .addr.boolean = &oneTimeDownload},
        {"protected", t_boolean, .addr.boolean = &protected},
        {NULL}
    };

    struct json_attr_t rStr_attrs[] = {
        {"token", t_string, .addr.string = token, .len = sizeof(token)},
        {"bucket", t_string, .addr.string = bucket, .len = sizeof(bucket)},
        {"url", t_string, .addr.string = url, .len = sizeof(url)},
        {"retentionPeriod", t_string, .addr.string = retentionPeriod, .len = sizeof(retentionPeriod)},
        {"options", t_object, .addr.attrs = options_attrs},
        {NULL}
    };

    struct json_attr_t rInt_attrs[] = {
        {"token", t_string, .addr.string = token, .len = sizeof(token)},
        {"bucket", t_string, .addr.string = bucket, .len = sizeof(bucket)},
        {"url", t_string, .addr.string = url, .len = sizeof(url)},
        {"retentionPeriod", t_integer, .addr.integer = &retentionPeriodInt},
        {"options", t_object, .addr.attrs = options_attrs},
        {NULL}
    };

    // Fix :null to :"null"
    adjustedBody[0] = 0;
    for(int i=0; i<strlen(body); i++) {
        if (body[i] == 'n' && body[i-1] == ':') {
            strcat(adjustedBody, "\"n");
        }
        else if (body[i] == ',' && body[i-1] == 'l') {
            strcat(adjustedBody, "\",");
        }
        else {
            char tmp[2];
            tmp[0] = body[i];
            tmp[1] = 0;
            strcat(adjustedBody, tmp);
        }
    }

    jsonStatus = json_read_object(adjustedBody, stringRetention ? rStr_attrs : rInt_attrs, NULL);
    free(adjustedBody);
    if(jsonStatus!=0) {
        fprintf(stderr, "json deserialize failed: %s\n", json_error_string(jsonStatus));
        fprintf(stderr, "raw body: %s\n", body);
        fprintf(stderr, "body size: %lu\n", strlen(body));
    };

    if(jsonStatus==0 && stringRetention) {
        retopts = CreateFileOptions(hasFilename, oneTimeDownload, protected);
        if (strcmp(bucket, "null")==0) strcpy(bucket, "");
        retval = CreateFileResponse(token, bucket, url, retentionPeriod, retopts);
    };

    if(jsonStatus==0 && !stringRetention) {
        retopts = CreateFileOptions(hasFilename, oneTimeDownload, protected);
        sprintf(retentionPeriod, "%d", retentionPeriodInt);
        if (strcmp(bucket, "null")==0) strcpy(bucket, "");
        retval = CreateFileResponse(token, bucket, url, retentionPeriod, retopts);
    };

    return retval;
}

BucketResponse deserializeBucketResponse(char *body) {
    int jsonStatus = 0, files_count = 0;
    BucketResponse retval;

    struct json_attr_t options_attrs[] = {
        {"hideFilename", t_ignore},
        {"oneTimeDownload", t_ignore},
        {"protected", t_ignore},
        {NULL}
    };

    struct json_attr_t files_attrs[] = {
        {"token", t_string, STRUCTOBJECT(struct FileResponse, token), .len = sizeof(retval.files[0].token)},
        {"bucket", t_string, STRUCTOBJECT(struct FileResponse, bucket), .len = sizeof(retval.files[0].bucket)},
        {"url", t_string, STRUCTOBJECT(struct FileResponse, url), .len = sizeof(retval.files[0].url)},
        {"retentionPeriod", t_ignore},
        {"options", t_object, .addr.attrs = options_attrs},
        {NULL}
    };

    struct json_attr_t bucket_attrs[] = {
        {"token", t_string, .addr.string = retval.token, .len = sizeof(retval.token)},
        {"files", t_array, STRUCTARRAY(retval.files, files_attrs, &files_count)},
        {NULL}
    };

    memset(&retval.files, '\0', sizeof(retval.files));

    jsonStatus = json_read_object(body, bucket_attrs, NULL);
    if(jsonStatus!=0) {
        fprintf(stderr, "json deserialize failed: %s\n", json_error_string(jsonStatus));
        fprintf(stderr, "raw body: %s\n", body);
        fprintf(stderr, "body size: %lu\n", strlen(body));
    };

    return retval;
}

RestrictionResponse deserializeRestrictionResponse(char *body) {
    int jsonStatus = 0, restriction_count = 0;
    char *adjustedBody = (char *)malloc(strlen(body)+100);
    RestrictionResponse retval;

    struct json_attr_t restriction_attrs[] = {
        {"type", t_string, STRUCTOBJECT(struct Restriction, type), .len = sizeof(retval.restrictions[0].type)},
        {"value", t_string, STRUCTOBJECT(struct Restriction, value), .len = sizeof(retval.restrictions[0].value)},
        {NULL}
    };

    struct json_attr_t restrictions_attrs[] = {
        {"restrictions", t_array, STRUCTARRAY(retval.restrictions, restriction_attrs, &restriction_count)},
        {NULL}
    };

    strcpy(adjustedBody, "{\"restrictions\":");
    for(int i=0; i<strlen(body); i++) {
        if (body[i] == ':' && body[i+1] != '"') {
            strcat(adjustedBody, ":\"");
        }
        else if (body[i] == '}' && body[i-1] != '"') {
            strcat(adjustedBody, "\"}");
        }
        else {
            char tmp[2];
            tmp[0] = body[i];
            tmp[1] = 0;
            strcat(adjustedBody, tmp);
        }
    }
    strcat(adjustedBody, "}");

    memset(&retval.restrictions, '\0', sizeof(retval.restrictions));

    jsonStatus = json_read_object(adjustedBody, restrictions_attrs, NULL);
    if(jsonStatus!=0) {
        fprintf(stderr, "json deserialize failed: %s\n", json_error_string(jsonStatus));
        fprintf(stderr, "raw body: %s\n", body);
        fprintf(stderr, "body size: %lu\n", strlen(body));
    };

    free(adjustedBody);
    return retval;
}

#endif //WAIFUVAULT_C_DESERIALIZERS_H
