// Waifuvault C SDK Models
#include<stdbool.h>
#include<string.h>
#include<strings.h>
#include<stdio.h>
#include<stdlib.h>

#ifndef WAIFUVAULT_C_MODELS
#define WAIFUVAULT_C_MODELS
// FileUpload

typedef struct FileUpload {
    char filename[512];
    char url[4096];
	char bucketToken[80];
    void *buffer;
    long bufferSize;
    char expires[10];
    char password[512];
    bool hideFilename;
    bool oneTimeDownload;
} FileUpload;

// FileOptions

typedef struct FileOptions {
    bool hasFilename;
    bool oneTimeDownload;
    bool protected;
} FileOptions;

// FileResponse

typedef struct FileResponse {
    char token[80];
	char bucket[80];
    char url[4096];
    char retentionPeriod[80];
    FileOptions options;
} FileResponse;

// BucketResponse

typedef struct BucketResponse {
    char token[80];
    FileResponse files[100];
} BucketResponse;

// ErrorResponse

typedef struct ErrorResponse {
    char name[80];
    int status;
    char message[4096];
} ErrorResponse;

// Restriction

typedef struct Restriction {
    char type[80];
    char value[512];
} Restriction;

// Restriction Response

typedef struct RestrictionResponse {
    Restriction restrictions[100];
} RestrictionResponse;

// Memory Stream for curl

typedef struct MemoryStream {
    char *memory;
    size_t size;
} MemoryStream;

#endif