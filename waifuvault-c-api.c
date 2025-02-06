// Waifuvault C SDK
#include "waifuvault-c-api.h"
#include "waifuvault-c-models.h"
#include "waifuvault-c-utils.h"
#include "waifuvault-c-deserializers.h"
#include "cJSON.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stddef.h>
#include<string.h>
#include<sys/stat.h>
#include<curl/curl.h>

static char *BASEURL  = "https://waifuvault.moe/rest";

static CURL *curl;
static ErrorResponse *error;
static RestrictionResponse restrictions;

// CURL Handling
void openCurl() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    error = NULL;
};

void closeCurl() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

CURLcode dispatchCurl(char *targetUrl, char *targetMethod, char *fields, struct curl_httppost *formpost, struct curl_slist *headers, MemoryStream *contents) {
    curl = curl_easy_init();
    contents->memory = malloc(1);
    contents->size = 0;

    curl_easy_setopt(curl, CURLOPT_URL, targetUrl);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, targetMethod);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryStream);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)contents);
    if(headers) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    if(formpost) curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    if(fields) curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields);
    return curl_easy_perform(curl);
}

// Error Handling
ErrorResponse *getError() {
    return error;
}

void clearError() {
    free(error);
    error = NULL;
}

bool checkError(CURLcode resp, char *body) {
    char name[80];
    char message[512];
    int status = 0;
    long http_code = 0;

    if(resp != CURLE_OK) {
        fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(resp));
        return true;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if(http_code >= 400) {
        fprintf(stderr, "http code error: %d\n", (int)http_code);
        if(error == NULL) error = (ErrorResponse *)malloc(sizeof(ErrorResponse));
        *error = deserializeErrorResponse(body);
        return true;
    }
    return false;
}

// Alt URL
void setAltBaseURL(const char *newBaseURL) {
    BASEURL = malloc(strlen(newBaseURL)+1);
    if (!BASEURL) {
        printf("MALLOC failed assigning base url: %s\n", newBaseURL);
        exit(1);
    }
    strcpy(BASEURL, newBaseURL);
}

// Restrictions
RestrictionResponse getRestrictions() {
    char url[512];
    MemoryStream contents;
    RestrictionResponse retval;

    sprintf(url, "%s/resources/restrictions", BASEURL);

    const CURLcode res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeRestrictionResponse(contents.memory);
    }
    free(contents.memory);
    for(int i = 0; i < 100; i++) {
        restrictions.restrictions[i] = retval.restrictions[i];
    }
    return retval;
}

FilesInfo getFileStats() {
    char url[512];
    MemoryStream contents;
    FilesInfo retval;

    sprintf(url, "%s/resources/stats/files", BASEURL);

    const CURLcode res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeFilesInfo(contents.memory);
    }
    free(contents.memory);
    return retval;
}

RestrictionResponse clearRestrictions() {
    RestrictionResponse retval;
    for(int i = 0; i < 100; i++) {
        strcpy(restrictions.restrictions[i].type,"");
        strcpy(restrictions.restrictions[i].value,"");
        strcpy(retval.restrictions[i].type,"");
        strcpy(retval.restrictions[i].value,"");
    }
    return retval;
}

bool checkRestrictions(FileUpload fileObj) {
    long filesize = 0;
    struct stat stats;
    char *endptr, *ext, *filemime;

    if(strlen(fileObj.url)==0) {
        if(strlen(fileObj.filename)>0 && fileObj.buffer==NULL) {
            //File Upload
            stat(expandHomedir(fileObj.filename), &stats);
            filesize = stats.st_size;
        } else {
            //Buffer Upload
            filesize = fileObj.bufferSize;
        }

        for(int i=0; i<100; i++) {
            if(strlen(restrictions.restrictions[i].type)==0) break;
            if(strcmp(restrictions.restrictions[i].type, "MAX_FILE_SIZE") == 0) {
                const long maxsize = strtol(restrictions.restrictions[i].value, &endptr, 10);
                if(filesize > maxsize) {
                    if(error == NULL) error = (ErrorResponse *)malloc(sizeof(ErrorResponse));
                    error->status = 1;
                    strcpy(error->name, "Restriction Error");
                    strcpy(error->message, "File size greater than maximum allowed by server");
                    return true;
                }
            }
            else if(strcmp(restrictions.restrictions[i].type,"BANNED_MIME_TYPE")==0) {
                ext = fileExtension(fileObj.filename);
                filemime = getMime(ext);
                if(strstr(restrictions.restrictions[i].value,filemime) != NULL) {
                    if(error == NULL) error = (ErrorResponse *)malloc(sizeof(ErrorResponse));
                    error->status = 1;
                    strcpy(error->name, "Restriction Error");
                    strcpy(error->message, "File mime type not allowed by server");
                    return true;
                }
            }
        }
    }
    return false;
}

// Albums

AlbumResponse createAlbum(char *bucketToken, char *name) {
    char url[512];
    char body[512];
    struct curl_slist *headers = NULL;
    MemoryStream contents;
    AlbumResponse retval;

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(url, "%s/album/%s", BASEURL, bucketToken);
    sprintf(body, "{\"name\":\"%s\"}", name);

    const CURLcode res = dispatchCurl(url, "POST", body, NULL, headers, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeAlbumResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

bool deleteAlbum(char *albumToken, bool deleteFiles) {
    char url[512];
    MemoryStream contents;

    sprintf(url, "%s/album/%s?deleteFiles=%s", BASEURL, albumToken, deleteFiles ? "true" : "false");

    const CURLcode res = dispatchCurl(url, "DELETE", NULL, NULL, NULL, &contents);

    if(checkError(res,contents.memory)) return false;
    const GeneralResponse resp = unmarshalGeneralResponse(ParseJson(contents.memory));
    free(contents.memory);
    return resp.success;
}

AlbumResponse getAlbum(char *token) {
    char url[512];
    MemoryStream contents;
    AlbumResponse retval;

    sprintf(url, "%s/album/%s", BASEURL, token);

    const CURLcode res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeAlbumResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

AlbumResponse associateFiles(char *token, char *fileTokens[], int count) {
    char url[512];
    char body[4096];
    struct curl_slist *headers = NULL;
    MemoryStream contents;
    AlbumResponse retval;

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(url, "%s/album/%s/associate", BASEURL, token);
    sprintf(body, "{\"fileTokens\": [");
    for(int i = 0; i<count; i++) {
        strcat(body, "\"");
        strcat(body,fileTokens[i]);
        strcat(body,"\",");
    }
    body[strlen(body)-1] = 0;
    strcat(body, "]}");

    const CURLcode res = dispatchCurl(url, "POST", body, NULL, headers, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeAlbumResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

AlbumResponse disassociateFiles(char *token, char *fileTokens[], int count) {
    char url[512];
    char body[4096];
    struct curl_slist *headers = NULL;
    MemoryStream contents;
    AlbumResponse retval;

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(url, "%s/album/%s/disassociate", BASEURL, token);
    sprintf(body, "{\"fileTokens\":[");
    for(int i = 0; i<count; i++) {
        strcat(body, "\"");
        strcat(body,fileTokens[i]);
        strcat(body,"\",");
    }
    body[strlen(body)-1] = 0;
    strcat(body, "]}");

    const CURLcode res = dispatchCurl(url, "POST", body, NULL, headers, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeAlbumResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

char *shareAlbum(char *token) {
    char url[512];
    MemoryStream contents;
    GeneralResponse resp;
    char *retval;

    sprintf(url, "%s/album/share/%s", BASEURL, token);

    const CURLcode res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        resp = unmarshalGeneralResponse(ParseJson(contents.memory));
        retval = malloc(strlen(resp.description) + 1);
        strcpy(retval, resp.description);
    }
    free(contents.memory);
    return retval;
}

bool revokeAlbum(char *token) {
    char url[512];
    MemoryStream contents;
    GeneralResponse resp;

    sprintf(url, "%s/album/revoke/%s", BASEURL, token);

    const CURLcode res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        resp = unmarshalGeneralResponse(ParseJson(contents.memory));
    }
    free(contents.memory);
    return resp.success;
}

void downloadAlbum(char *token, int *files, int count, MemoryStream *contents) {
    char url[512];
    char body[4096];
    struct curl_slist *headers = NULL;

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(url, "%s/album/download/%s", BASEURL, token);
    if(count == 0) {
        sprintf(body, "[]");
    }
    else {
        sprintf(body, "[");
        for(int i=0; i<count; i++) {
            char str[20];
            snprintf(str, sizeof(str), "%d,", files[i]);
            strcat(body, str);
        }
        strcat(body, "]");
    }

    const CURLcode res = dispatchCurl(url, "POST", body, NULL, headers, contents);
    if(checkError(res,contents->memory)) return;
}

// Buckets
BucketResponse createBucket() {
    char url[512];
    MemoryStream contents;
    BucketResponse retval;

    sprintf(url, "%s/bucket/create", BASEURL);

    const CURLcode res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeBucketResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

bool deleteBucket(char *token) {
    char url[512];
    MemoryStream contents;

    sprintf(url, "%s/bucket/%s", BASEURL, token);

    const CURLcode res = dispatchCurl(url, "DELETE", NULL, NULL, NULL, &contents);

    if(checkError(res,contents.memory)) return false;
    const bool retval = strncmp(contents.memory,"true",4)==0;
    free(contents.memory);
    return retval;
}

BucketResponse getBucket(char *token) {
    char url[512];
    char body[512];
    struct curl_slist *headers = NULL;
    MemoryStream contents;
    BucketResponse retval;

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(url, "%s/bucket/get", BASEURL);
    sprintf(body, "{\"bucket_token\":\"%s\"}", token);

    const CURLcode res = dispatchCurl(url, "POST", body, NULL, headers, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeBucketResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

// Files
FileResponse uploadFile(FileUpload fileObj) {
    FileResponse retval;
    CURLcode res;
    MemoryStream contents;
    char fields[120], initUrl[1024];
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;

    curl = curl_easy_init();
    contents.memory = malloc(1);
    contents.size = 0;
    if(!curl) return retval;

    if(checkRestrictions(fileObj)) return retval;

    strcpy(initUrl, BASEURL);
    if(strlen(fileObj.bucketToken)>0) {
        strcat(initUrl,"/");
        strcat(initUrl,fileObj.bucketToken);
    }

    char *targetUrl = BuildURL(initUrl, fileObj);
    if(strlen(fileObj.url)>0) {
        // URL Upload
        strcat(fields, "url=");
        strcat(fields, curl_easy_escape(curl, fileObj.url, strlen(fileObj.url)));

        if (strlen(fileObj.password) > 0) {
            strcat(fields, "&password=");
            strcat(fields, curl_easy_escape(curl, fileObj.password, strlen(fileObj.password)));
        }

        res = dispatchCurl(targetUrl, "PUT", fields, NULL, NULL, &contents);
    }
    else if(strlen(fileObj.filename)>0 && fileObj.buffer==NULL) {
        // File Upload
        curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "file",
               CURLFORM_FILE, expandHomedir(fileObj.filename),
               CURLFORM_END);

        if (strlen(fileObj.password) > 0) {
            curl_formadd(&formpost,
                         &lastptr,
                         CURLFORM_COPYNAME, "password",
                         CURLFORM_COPYCONTENTS, fileObj.password,
                         CURLFORM_END);
        }

        res = dispatchCurl(targetUrl, "PUT", NULL, formpost, NULL, &contents);
        curl_formfree(formpost);
    }
    else {
        // Buffer upload
        curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "file",
               CURLFORM_BUFFER, fileObj.filename,
               CURLFORM_BUFFERPTR, fileObj.buffer,
               CURLFORM_BUFFERLENGTH, fileObj.bufferSize,
               CURLFORM_END);

        if (strlen(fileObj.password) > 0) {
            curl_formadd(&formpost,
                         &lastptr,
                         CURLFORM_COPYNAME, "password",
                         CURLFORM_COPYCONTENTS, fileObj.password,
                         CURLFORM_END);
        }

        res = dispatchCurl(targetUrl, "PUT", NULL, formpost, NULL, &contents);
        curl_formfree(formpost);
    }

    if(!checkError(res, contents.memory)) {
        retval = deserializeResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

FileResponse fileInfo(char *token, bool formatted) {
    FileResponse retval;
    char url[120];
    MemoryStream contents;

    sprintf(url, "%s/%s?formatted=%s", BASEURL, token, formatted ? "true" : "false");

    const CURLcode res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res,contents.memory)) {
        retval = deserializeResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

FileResponse fileUpdate(char *token, char *password, char *previousPassword, char *customExpiry, bool hideFilename) {
    FileResponse retval;
    char url[120];
    char fields[120];
    MemoryStream contents;
    struct curl_slist *headers = NULL;

    sprintf(url, "%s/%s", BASEURL, token);

    sprintf(fields, "{\"password\":\"");
    if(strlen(password)>0) strcat(fields, password);
    strcat(fields, "\",\"previousPassword\":\"");
    if(strlen(previousPassword)>0) strcat(fields, previousPassword);
    strcat(fields,"\",\"customExpiry\":\"");
    if(strlen(customExpiry)>0) strcat(fields,customExpiry);
    strcat(fields, "\",\"hideFilename\":");
    hideFilename ? strcat(fields,"true") : strcat(fields,"false");
    strcat(fields, "}");

    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
    const CURLcode res = dispatchCurl(url, "PATCH", fields, NULL, headers, &contents);

    curl_slist_free_all(headers);
    if(!checkError(res,contents.memory)) {
        retval = deserializeResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

bool deleteFile(char *token) {
    char url[120];
    MemoryStream contents;

    sprintf(url, "%s/%s", BASEURL, token);

    const CURLcode res = dispatchCurl(url, "DELETE", NULL, NULL, NULL, &contents);

    if(checkError(res,contents.memory)) return false;
    const bool retval = strncmp(contents.memory,"true",4)==0;
    free(contents.memory);
    return retval;
}

void getFile(FileResponse fileObj, MemoryStream *contents, char *password) {
    char xpassword[80];
    struct curl_slist *headers = NULL;

    if(strlen(fileObj.url)==0 && strlen(fileObj.token)>0) {
        FileResponse fileUrl = fileInfo(fileObj.token, false);
        strcpy(fileObj.url, fileUrl.url);
    }

    if(strlen(password)>0) {
        sprintf(xpassword,"x-password: ");
        strcat(xpassword, password);
        headers = curl_slist_append(headers, xpassword);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    };

    const CURLcode res = dispatchCurl(fileObj.url, "GET", NULL, NULL, headers ? headers : NULL, contents);

    if(headers) curl_slist_free_all(headers);
    if(checkError(res,contents->memory)) return;
}
