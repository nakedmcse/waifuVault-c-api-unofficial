// Waifuvault C SDK
#include "waifuvault-c-api.h"
#include "waifuvault-c-models.h"
#include "waifuvault-c-utils.h"
#include "mjson.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<stddef.h>
#include<string.h>
#include<sys/stat.h>
#include<strings.h>
#include<curl/curl.h>

#define BASEURL "https://waifuvault.moe/rest"

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
    int jsonStatus = 0;
    long http_code = 0;

    struct json_attr_t error_attrs[] = {
        {"name", t_string, .addr.string = name, .len = sizeof(name)},
        {"status", t_integer, .addr.integer = &status},
        {"message", t_string, .addr.string = message, .len = sizeof(message)},
        {NULL}
    };

    if(resp != CURLE_OK) {
        fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(resp));
        return true;
    }
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    if(http_code >= 400) {
        fprintf(stderr, "http code error: %d\n", (int)http_code);

        jsonStatus = json_read_object(body, error_attrs, NULL);
        if(jsonStatus!=0) {
            fprintf(stderr, "json deserialize failed: %s\n", json_error_string(jsonStatus));
            fprintf(stderr, "raw body: %s\n", body);
            fprintf(stderr, "body size: %d\n", strlen(body));
            return true;
        };

        if(error == NULL) error = (ErrorResponse *)malloc(sizeof(ErrorResponse));
        error->status = status;
        strcpy(error->name, name);
        strcpy(error->message, message);

        return true;
    }
    return false;
}

// Restrictions
RestrictionResponse getRestrictions() {
    char url[512];
    CURLcode res;
    MemoryStream contents;
    RestrictionResponse retval;

    sprintf(url, "%s/resources/restrictions", BASEURL);

    res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeRestrictionResponse(contents.memory);
    }
    free(contents.memory);
    for(int i = 0; i < 100; i++) {
        restrictions.restrictions[i] = retval.restrictions[i];
    }
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
    long filesize, maxsize = 0;
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
                maxsize = strtol(restrictions.restrictions[i].value, &endptr, 10);
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

// Buckets
BucketResponse createBucket() {
    char url[512];
    CURLcode res;
    MemoryStream contents;
    BucketResponse retval;

    sprintf(url, "%s/bucket/create", BASEURL);

    res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeBucketResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

bool deleteBucket(char *token) {
    char url[512];
    CURLcode res;
    MemoryStream contents;
    bool retval;

    sprintf(url, "%s/bucket/%s", BASEURL, token);

    res = dispatchCurl(url, "DELETE", NULL, NULL, NULL, &contents);

    if(checkError(res,contents.memory)) return false;
    retval = strncmp(contents.memory,"true",4)==0;
    free(contents.memory);
    return retval;
}

BucketResponse getBucket(char *token) {
    char url[512];
    char body[512];
    struct curl_slist *headers = NULL;
    CURLcode res;
    MemoryStream contents;
    BucketResponse retval;

    headers = curl_slist_append(headers, "Content-Type: application/json");
    sprintf(url, "%s/bucket/get", BASEURL);
    sprintf(body, "{\"bucket_token\":\"%s\"}", token);

    res = dispatchCurl(url, "POST", body, NULL, headers, &contents);
    if(!checkError(res, contents.memory)) {
        retval = deserializeBucketResponse(contents.memory);
    }
    free(contents.memory);
    return retval;
}

// Files
FileResponse uploadFile(FileUpload fileObj) {
    FileResponse retval;
    char *targetUrl;
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

    targetUrl = BuildURL(initUrl, fileObj);
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
        retval = deserializeResponse(contents.memory, true);
    }
    free(contents.memory);
    return retval;
}

FileResponse fileInfo(char *token, bool formatted) {
    FileResponse retval;
    char url[120];
    CURLcode res;
    MemoryStream contents;

    sprintf(url, "%s/%s?formatted=%s", BASEURL, token, formatted ? "true" : "false");

    res = dispatchCurl(url, "GET", NULL, NULL, NULL, &contents);
    if(!checkError(res,contents.memory)) {
        retval = deserializeResponse(contents.memory, formatted);
    }
    free(contents.memory);
    return retval;
}

FileResponse fileUpdate(char *token, char *password, char *previousPassword, char *customExpiry, bool hideFilename) {
    FileResponse retval;
    char url[120];
    char fields[120];
    CURLcode res;
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
    res = dispatchCurl(url, "PATCH", fields, NULL, headers, &contents);

    curl_slist_free_all(headers);
    if(!checkError(res,contents.memory)) {
        retval = deserializeResponse(contents.memory, false);
    }
    free(contents.memory);
    return retval;
}

bool deleteFile(char *token) {
    char url[120];
    CURLcode res;
    MemoryStream contents;
    bool retval;

    sprintf(url, "%s/%s", BASEURL, token);

    res = dispatchCurl(url, "DELETE", NULL, NULL, NULL, &contents);

    if(checkError(res,contents.memory)) return false;
    retval = strncmp(contents.memory,"true",4)==0;
    free(contents.memory);
    return retval;
}

void getFile(FileResponse fileObj, MemoryStream *contents, char *password) {
    CURLcode res;
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

    res = dispatchCurl(fileObj.url, "GET", NULL, NULL, headers ? headers : NULL, contents);

    if(headers) curl_slist_free_all(headers);
    if(checkError(res,contents->memory)) return;
}

// Deserializers
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
        fprintf(stderr, "body size: %d\n", strlen(body));
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
