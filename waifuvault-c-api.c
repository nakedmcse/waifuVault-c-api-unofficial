// Waifuvault C SDK
#include "waifuvault-c-api.h"
#include "waifuvault-c-models.h"
#include "mjson.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<string.h>
#include<strings.h>
#include<curl/curl.h>

#define BASEURL "https://waifuvault.walker.moe/rest"

static CURL *curl;

void openCurl() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
};

void closeCurl() {
    curl_easy_cleanup(curl);
    curl_global_cleanup();
}

FileResponse uploadFile(FileUpload fileObj) {
    FileResponse retval;
    char *targetUrl;
    void *content;
    CURLcode res;
    MemoryStream contents;
    char fields[120];
    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;

    curl = curl_easy_init();
    contents.memory = malloc(1);
    contents.size = 0;
    if(!curl) return retval;

    targetUrl = BuildURL(BASEURL, fileObj);
    if(strlen(fileObj.url)>0) {
        // URL Upload
        strcat(fields, "url=");
        strcat(fields, curl_easy_escape(curl, fileObj.url, strlen(fileObj.url)));
        curl_easy_setopt(curl, CURLOPT_URL, targetUrl);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryStream);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&contents);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields);
        res = curl_easy_perform(curl);
    }
    else if(strlen(fileObj.filename)>0 && !fileObj.buffer) {
        // File Upload
        curl_formadd(&formpost,
               &lastptr,
               CURLFORM_COPYNAME, "file",
               CURLFORM_FILE, fileObj.filename,
               CURLFORM_CONTENTTYPE, "octet-stream",
               CURLFORM_END);
        curl_easy_setopt(curl, CURLOPT_URL, targetUrl);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryStream);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&contents);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        res = curl_easy_perform(curl);
        curl_formfree(formpost);
    }
    else {
        // Buffer upload
        // Load buffer to multipart upload content as file,<file>
        retval = sendContent(targetUrl, content);
    }

    if(!checkError(res)) {
        retval = deserializeResponse(contents.memory, true);
    }
    return retval;
}

FileResponse fileInfo(char *token, bool formatted) {
    FileResponse retval;
    char url[120];
    CURLcode res;
    MemoryStream contents;

    sprintf(url, "%s/%s?formatted=%s", BASEURL, token, formatted ? "true" : "false");
    contents.memory = malloc(1);
    contents.size = 0;
    curl = curl_easy_init();

    if(!curl) return retval;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryStream);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&contents);
    res = curl_easy_perform(curl);
    if(!checkError(res)) {
        retval = deserializeResponse(contents.memory, formatted);
    }
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
    contents.memory = malloc(1);
    contents.size = 0;
    curl = curl_easy_init();
    if(!curl) return retval;

    strcat(fields, "{\"password\":\"");
    if(strlen(password)>0) strcat(fields, password);
    strcat(fields, "\",\"previousPassword\":\"");
    if(strlen(previousPassword)>0) strcat(fields, previousPassword);
    strcat(fields,"\",\"customExpiry\":\"");
    if(strlen(customExpiry)>0) strcat(fields,customExpiry);
    strcat(fields, "\",\"hideFilename\":");
    hideFilename ? strcat(fields,"true") : strcat(fields,"false");
    strcat(fields, "}");

    headers = curl_slist_append(headers, "Content-Type: application/json; charset=utf-8");
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryStream);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&contents);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, fields);
    res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    if(!checkError(res)) {
        retval = deserializeResponse(contents.memory, false);
    }
    return retval;
}

bool deleteFile(char *token) {
    char url[120];
    CURLcode res;
    MemoryStream contents;

    sprintf(url, "%s/%s", BASEURL, token);
    contents.memory = malloc(1);
    contents.size = 0;
    curl = curl_easy_init();

    if(!curl) return false;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryStream);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&contents);
    res = curl_easy_perform(curl);

    if(checkError(res)) return false;
    return strncmp(contents.memory,"true",4)==0;
}

void *getFile(FileResponse fileObj, char *password) {
    CURLcode res;
    MemoryStream contents;
    char xpassword[80];
    struct curl_slist *headers = NULL;

    if(strlen(fileObj.url)==0 && strlen(fileObj.token)>0) {
        FileResponse fileUrl = fileInfo(fileObj.token, false);
        strcpy(fileObj.url, fileUrl.url);
    }

    contents.memory = malloc(1);
    contents.size = 0;
    curl = curl_easy_init();
    if(!curl) return NULL;

    if(strlen(password)>0) {
        strcat(xpassword,"x-password: ");
        strcat(xpassword, password);
        headers = curl_slist_append(headers, xpassword);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    };
    curl_easy_setopt(curl, CURLOPT_URL, fileObj.url);
    curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryStream);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&contents);
    res = curl_easy_perform(curl);

    if(headers) curl_slist_free_all(headers);
    if(!checkError(res)) return NULL;

    // TODO: Pass a Memorystream object back
    return contents.memory;
}

FileResponse sendContent(char *targetUrl, void *content) {
    FileResponse retval;
    // PUT request to URL
    // Check for errors
    // Deserialize to retval
    return retval;
}

bool checkError(CURLcode resp) {
    if(resp != CURLE_OK) {
        fprintf(stderr, "curl failed: %s\n", curl_easy_strerror(resp));
        return true;
    }
    return false;
}

FileResponse deserializeResponse(char *body, bool stringRetention) {
    char token[80];
    char url[120];
    char retentionPeriod[80];
    int retentionPeriodInt;
    int jsonStatus = 0;
    bool hasFilename;
    bool oneTimeDownload;
    bool protected;
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
        {"url", t_string, .addr.string = url, .len = sizeof(url)},
        {"retentionPeriod", t_string, .addr.string = retentionPeriod, .len = sizeof(retentionPeriod)},
        {"options", t_object, .addr.attrs = options_attrs},
        {NULL}
    };

    struct json_attr_t rInt_attrs[] = {
        {"token", t_string, .addr.string = token, .len = sizeof(token)},
        {"url", t_string, .addr.string = url, .len = sizeof(url)},
        {"retentionPeriod", t_integer, .addr.integer = &retentionPeriodInt},
        {"options", t_object, .addr.attrs = options_attrs},
        {NULL}
    };

    jsonStatus = json_read_object(body, stringRetention ? rStr_attrs : rInt_attrs, NULL);
    if(jsonStatus!=0) {
        fprintf(stderr, "json deserialize failed: %s\n", json_error_string(jsonStatus));
        fprintf(stderr, "raw body: %s\n", body);
        fprintf(stderr, "body size: %d\n", strlen(body));
    };

    if(jsonStatus==0 && stringRetention) {
        retopts = CreateFileOptions(hasFilename, oneTimeDownload, protected);
        retval = CreateFileResponse(token, url, retentionPeriod, retopts);
    };

    if(jsonStatus==0 && !stringRetention) {
        retopts = CreateFileOptions(hasFilename, oneTimeDownload, protected);
        sprintf(retentionPeriod, "%d", retentionPeriodInt);
        retval = CreateFileResponse(token, url, retentionPeriod, retopts);
    };

    return retval;
}

// Builder helper functions

char* BuildURL(char *baseURL, FileUpload upload) {
    char *retval;
    retval = (char *)malloc(4096);
    if (!retval) return NULL;  // malloc failed

    strcpy(retval, baseURL);
    strcat(retval, "?");

    char buffer[4096];

    if(strlen(upload.password)>0) {
        sprintf(buffer, "password=%s&", upload.password);
        strcat(retval, buffer);
    }

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
    strcpy(retval.expires,expires);
    strcpy(retval.password,password);
    if(strncasecmp(target, "http://", 7) == 0 || strncasecmp(target, "https://", 8) == 0) {
        // Url
        strcpy(retval.url, target);
    }
    else {
        // Filename
        strcpy(retval.filename, target);
    }
    return retval;
}

FileUpload CreateBufferUpload(void *target, char *expires, char *password, bool hidefilename, bool onetimedownload) {
    FileUpload retval;
    retval.buffer = target;
    retval.hideFilename = hidefilename;
    retval.oneTimeDownload = onetimedownload;
    strcpy(retval.expires,expires);
    strcpy(retval.password,password);
    return retval;
}

FileOptions CreateFileOptions(bool hasfilename, bool onetimedownload, bool protected) {
    FileOptions retval;
    retval.hasFilename = hasfilename;
    retval.oneTimeDownload = onetimedownload;
    retval.protected = protected;
    return retval;
}

FileResponse CreateFileResponse(char *token, char *url, char *retention, FileOptions options) {
    FileResponse retval;
    if(token != NULL) strcpy(retval.token, token);
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

// Memorystream implementation

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


