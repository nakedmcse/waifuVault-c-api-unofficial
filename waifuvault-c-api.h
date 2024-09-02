// Waifuvault C SDK prototypes
#include<stdbool.h>
#include "waifuvault-c-models.h"
#include<curl/curl.h>
#ifndef WAIFUVAULT_C_API
#define WAIFUVAULT_C_API

void openCurl();
void closeCurl();
ErrorResponse *getError();
void clearError();
RestrictionResponse getRestrictions();
RestrictionResponse clearRestrictions();
BucketResponse createBucket();
bool deleteBucket(char *token);
BucketResponse getBucket(char *token);
FileResponse uploadFile(FileUpload fileObj);
FileResponse fileInfo(char *token, bool formatted);
FileResponse fileUpdate(char *token, char *password, char *previousPassword, char *customExpiry, bool hideFilename);
bool deleteFile(char *token);
void getFile(FileResponse fileObj, MemoryStream *contents, char *password);
bool checkError(CURLcode resp, char *body);
bool checkRestrictions(FileUpload fileObj);
void setAltBaseURL(const char *newBaseURL);
FileResponse deserializeResponse(char *body, bool stringRetention);
BucketResponse deserializeBucketResponse(char *body);
RestrictionResponse deserializeRestrictionResponse(char *body);
char* BuildURL(char *baseURL, FileUpload upload);
FileUpload CreateFileUpload(char *target, char *expires, char *password, bool hidefilename, bool onetimedownload);
FileUpload CreateBucketFileUpload(char *target, char *bucketToken, char *expires, char *password, bool hidefilename, bool onetimedownload);
FileUpload CreateBufferUpload(void *target, long size, char *filename, char *expires, char *password, bool hidefilename, bool onetimedownload);
FileOptions CreateFileOptions(bool hasfilename, bool onetimedownload, bool protected);
FileResponse CreateFileResponse(char *token, char *bucket, char *url, char *retention, FileOptions options);
ErrorResponse CreateErrorResponse(char *name, int status, char *message);
static size_t WriteMemoryStream(void *contents, size_t size, size_t membytes, void *userp);
char* expandHomedir(const char* path);
char *fileExtension(const char *filename);
char *getMime(const char *ext);

#endif
