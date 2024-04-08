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
    void *buffer;
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
    char url[4096];
    char retentionPeriod[80];
    FileOptions options;
} FileResponse;

// ErrorResponse

typedef struct ErrorResponse {
    char name[80];
    int status;
    char message[4096];
} ErrorResponse;

// Memory Stream for curl

typedef struct MemoryStream {
    char *memory;
    size_t size;
} MemoryStream;

#endif