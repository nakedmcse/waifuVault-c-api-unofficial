// Waifuvault C SDK Utility Functions
#include<stdbool.h>
#include<string.h>
#include<strings.h>
#include<stdio.h>
#include<stdlib.h>
#include "waifuvault-c-models.h"

#ifndef WAIFUVAULT_C_UTILS
#define WAIFUVAULT_C_UTILS

// Expand homedir

char* expandHomedir(const char* path) {
    const char* home = getenv("HOME");
    if (path[0] != '~' || !home) return strdup(path);

    size_t new_path_size = strlen(home) + strlen(path);
    char* new_path = malloc(new_path_size);
    if (!new_path) return strdup(path);
    strcpy(new_path, home);
    strcat(new_path, path + 1);
    return new_path;
}

// Get File Extension

char *fileExtension(const char *filename) {
    char *afterDot = strchr(filename, '.');
    if(afterDot == NULL || afterDot == filename) return "";
    return afterDot;
}

// Get Mime Type

char *getMime(const char *ext) {
    int i = 0;

    while(mimes[i].ext != NULL) {
        if(strcmp(ext, mimes[i].ext) == 0) {
            return mimes[i].mime;
        }
        i++;
    }

    return mimes[i].mime;  // NULL contains default
}

// Builder helper functions

char* BuildURL(char *baseURL, FileUpload upload) {
    char *retval;
    retval = (char *)malloc(4096);
    if (!retval) return NULL;  // malloc failed

    strcpy(retval, baseURL);
    strcat(retval, "?");

    char buffer[4096];

    if(strlen(upload.expires)>0) {
        sprintf(buffer, "expires=%s&", upload.expires);
        strcat(retval, buffer);
    }

    sprintf(buffer, "hide_filename=%s&", upload.hideFilename ? "true" : "false");
    strcat(retval, buffer);

    sprintf(buffer, "one_time_download=%s&", upload.oneTimeDownload ? "true" : "false");
    strcat(retval, buffer);

    if(retval[strlen(retval)-1] == '&') retval[strlen(retval)-1] = '\0';
    return retval;
}

FileUpload CreateFileUpload(char *target, char *expires, char *password, bool hidefilename, bool onetimedownload) {
    FileUpload retval;
    retval.hideFilename = hidefilename;
    retval.oneTimeDownload = onetimedownload;
    retval.buffer = NULL;
    strcpy(retval.expires,expires);
    strcpy(retval.password,password);
    strcpy(retval.bucketToken, "");
    if(strncasecmp(target, "http://", 7) == 0 || strncasecmp(target, "https://", 8) == 0) {
        // Url
        strcpy(retval.url, target);
        strcpy(retval.filename, "");
    }
    else {
        // Filename
        strcpy(retval.filename, target);
        strcpy(retval.url, "");
    }
    return retval;
}

FileUpload CreateBucketFileUpload(char *target, char *bucketToken, char *expires, char *password, bool hidefilename, bool onetimedownload) {
    FileUpload retval;
    retval.hideFilename = hidefilename;
    retval.oneTimeDownload = onetimedownload;
    retval.buffer = NULL;
    strcpy(retval.expires, expires);
    strcpy(retval.password, password);
    strcpy(retval.bucketToken, bucketToken);
    if(strncasecmp(target, "http://", 7) == 0 || strncasecmp(target, "https://", 8) == 0) {
        // Url
        strcpy(retval.url, target);
        strcpy(retval.filename, "");
    }
    else {
        // Filename
        strcpy(retval.filename, target);
        strcpy(retval.url, "");
    }
    return retval;
}

FileUpload CreateBufferUpload(void *target, long size, char *filename, char *expires, char *password, bool hidefilename, bool onetimedownload) {
    FileUpload retval;
    retval.buffer = target;
    retval.bufferSize = size;
    retval.hideFilename = hidefilename;
    retval.oneTimeDownload = onetimedownload;
    strcpy(retval.expires,expires);
    strcpy(retval.password,password);
    strcpy(retval.filename,filename);
    strcpy(retval.url, "");
    strcpy(retval.bucketToken, "");
    return retval;
}

FileOptions CreateFileOptions(bool hasfilename, bool onetimedownload, bool protected) {
    FileOptions retval;
    retval.hasFilename = hasfilename;
    retval.oneTimeDownload = onetimedownload;
    retval.protected = protected;
    return retval;
}

FileResponse CreateFileResponse(char *token, char *bucket, char *url, char *retention, FileOptions options) {
    FileResponse retval;
    if(token != NULL) strcpy(retval.token, token);
    if(bucket != NULL) strcpy(retval.bucket, bucket);
    if(url != NULL) strcpy(retval.url, url);
    if(retention != NULL) strcpy(retval.retentionPeriod, retention);
    retval.options = options;
    return retval;
}

ErrorResponse CreateErrorResponse(char *name, int status, char *message) {
    ErrorResponse retval;
    retval.status = status;
    if(name != NULL) strcpy(retval.name, name);
    if(message != NULL) strcpy(retval.message, message);
    return retval;
}

// MemoryStream implementation

static size_t WriteMemoryStream(void *contents, size_t size, size_t membytes, void *userp) {
    size_t realsize = size * membytes;
    MemoryStream *mem = (struct MemoryStream *)userp;

    char *ptr = realloc(mem->memory, mem->size + realsize + 1);
    if(!ptr) {
        printf("out of memory\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

#endif