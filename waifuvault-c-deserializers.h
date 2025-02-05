// Waifuvault C SDK JSON Desrializers
#include<stdio.h>
#include<stdbool.h>
#include<stdlib.h>
#include<stddef.h>
#include "waifuvault-c-models.h"
#include "cJSON.h"

#ifndef WAIFUVAULT_C_DESERIALIZERS_H
#define WAIFUVAULT_C_DESERIALIZERS_H

// Unmarshallers
// Deserialize FileOptions
FileOptions unmarshalFileOptions(cJSON *body) {
    FileOptions retval;
    cJSON *protected, *hasFilename, *oneTimeDownload;

    protected = cJSON_GetObjectItem(body, "protected");
    hasFilename = cJSON_GetObjectItem(body, "hasFilename");
    oneTimeDownload = cJSON_GetObjectItem(body, "oneTimeDownload");

    if(cJSON_IsBool(protected)) retval.protected = protected->valueint;
    if(cJSON_IsBool(hasFilename)) retval.hasFilename = hasFilename->valueint;
    if(cJSON_IsBool(oneTimeDownload)) retval.oneTimeDownload = oneTimeDownload->valueint;

    return retval;
}

// Deserialize AlbumInfo
AlbumInfo unmarshalAlbumInfo(cJSON *body) {
    AlbumInfo retval;
    cJSON *token, *publicToken, *name, *bucket, *dateCreated;

    token = cJSON_GetObjectItem(body, "token");
    publicToken = cJSON_GetObjectItem(body, "publicToken");
    name = cJSON_GetObjectItem(body, "name");
    bucket = cJSON_GetObjectItem(body, "bucket");
    dateCreated = cJSON_GetObjectItem(body, "dateCreated");

    if(cJSON_IsString(token)) strncpy(retval.token, token->valuestring, 80);
    if(cJSON_IsNull(publicToken)) strcpy(retval.publicToken, "");
    else if(cJSON_IsString(publicToken)) strncpy(retval.publicToken, publicToken->valuestring, 80);
    if(cJSON_IsString(bucket)) strncpy(retval.bucket, bucket->valuestring, 80);
    if(cJSON_IsString(name)) strncpy(retval.name, name->valuestring, 120);
    if(cJSON_IsNumber(dateCreated)) retval.dateCreated = (unsigned long)dateCreated->valueint;

    return retval;
}

// Deserialize FileResponse
FileResponse unmarshalFileResponse(cJSON *body) {
    FileResponse retval;
    AlbumInfo emptyAlbum;
    emptyAlbum.bucket[0] = 0;
    emptyAlbum.name[0] = 0;
    emptyAlbum.token[0] = 0;
    emptyAlbum.publicToken[0] = 0;
    emptyAlbum.dateCreated = 0;
    FileOptions emptyOptions;
    emptyOptions.protected = false;
    emptyOptions.hasFilename = false;
    emptyOptions.oneTimeDownload = false;
    cJSON *token, *bucket, *url, *retentionPeriod, *id, *views, *album, *options;

    token = cJSON_GetObjectItem(body, "token");
    bucket = cJSON_GetObjectItem(body, "bucket");
    url = cJSON_GetObjectItem(body, "url");
    retentionPeriod = cJSON_GetObjectItem(body, "retentionPeriod");
    id = cJSON_GetObjectItem(body, "id");
    views = cJSON_GetObjectItem(body, "views");
    album = cJSON_GetObjectItem(body, "album");
    options = cJSON_GetObjectItem(body, "options");

    if(cJSON_IsString(token)) strncpy(retval.token, token->valuestring, 80);
    if(cJSON_IsString(bucket)) strncpy(retval.bucket, bucket->valuestring, 80);
    if(cJSON_IsString(url)) strncpy(retval.url, url->valuestring, 4096);
    if(cJSON_IsString(retentionPeriod)) strncpy(retval.retentionPeriod, retentionPeriod->valuestring, 80);
    else if(cJSON_IsNumber(retentionPeriod)) sprintf(retval.retentionPeriod, "%d", retentionPeriod->valueint);
    if(cJSON_IsNumber(id)) retval.id = id->valueint;
    if(cJSON_IsNumber(views)) retval.views = views->valueint;
    if(!cJSON_IsNull(album)) retval.album = unmarshalAlbumInfo(album);
    else retval.album = emptyAlbum;
    if(!cJSON_IsNull(options)) retval.options = unmarshalFileOptions(options);
    else retval.options = emptyOptions;

    return retval;
}

// Deserialize AlbumResponse
AlbumResponse unmarshalAlbumResponse(cJSON *body) {
    AlbumResponse retval;
    cJSON *token, *publicToken, *name, *bucket, *dateCreated, *files, *file;
    int i = 0;

    token = cJSON_GetObjectItem(body, "token");
    publicToken = cJSON_GetObjectItem(body, "publicToken");
    name = cJSON_GetObjectItem(body, "name");
    bucket = cJSON_GetObjectItem(body, "bucketToken");
    dateCreated = cJSON_GetObjectItem(body, "dateCreated");
    files = cJSON_GetObjectItem(body, "files");

    if(cJSON_IsString(token)) strncpy(retval.token, token->valuestring, 80);
    if(cJSON_IsNull(publicToken)) strcpy(retval.publicToken, "");
    else if(cJSON_IsString(publicToken)) strncpy(retval.publicToken, publicToken->valuestring, 80);
    if(cJSON_IsString(bucket)) strncpy(retval.bucketToken, bucket->valuestring, 80);
    if(cJSON_IsString(name)) strncpy(retval.name, name->valuestring, 120);
    if(cJSON_IsNumber(dateCreated)) retval.dateCreated = (unsigned long)dateCreated->valueint;

    cJSON_ArrayForEach(file, files) {
        retval.files[i] = unmarshalFileResponse(file);
        i++;
        if(i>255) break;
    }

    return retval;
}

// DeserializeBucket
BucketResponse unmarshalBucket(cJSON *body) {
    BucketResponse retval;
    retval.token[0] = 0;
    AlbumInfo emptyAlbum;
    emptyAlbum.bucket[0] = 0;
    emptyAlbum.name[0] = 0;
    emptyAlbum.token[0] = 0;
    emptyAlbum.publicToken[0] = 0;
    emptyAlbum.dateCreated = 0;
    FileOptions emptyOptions;
    emptyOptions.protected = false;
    emptyOptions.hasFilename = false;
    emptyOptions.oneTimeDownload = false;
    FileResponse emptyFile;
    emptyFile.album = emptyAlbum;
    emptyFile.options = emptyOptions;
    emptyFile.bucket[0] = 0;
    emptyFile.id = -1;
    emptyFile.token[0] = 0;
    emptyFile.url[0] = 0;
    emptyFile.views = -1;
    emptyFile.retentionPeriod[0] = 0;
    retval.files[0] = emptyFile;
    retval.albums[0] = emptyAlbum;

    cJSON *token, *files, *file, *albums, *album;
    int i = 0, j = 0;
    token = cJSON_GetObjectItem(body, "token");
    files = cJSON_GetObjectItem(body, "files");
    albums = cJSON_GetObjectItem(body, "albums");

    if(cJSON_IsString(token)) strncpy(retval.token, token->valuestring, 80);
    cJSON_ArrayForEach(file, files) {
        retval.files[i] = unmarshalFileResponse(file);
        i++;
        if(i>255) break;
    }
    cJSON_ArrayForEach(album, albums) {
        retval.albums[j] = unmarshalAlbumInfo(album);
        j++;
        if(j>255) break;
    }

    return retval;
}

// Deserialize ErrorResponse
ErrorResponse unmarshalErrorResponse(cJSON *body) {
    ErrorResponse retval;
    retval.name[0] = 0;
    retval.message[0] = 0;
    retval.status = -1;
    cJSON *name, *status, *message;

    name = cJSON_GetObjectItem(body, "name");
    status = cJSON_GetObjectItem(body, "status");
    message = cJSON_GetObjectItem(body, "message");

    if(cJSON_IsString(name)) strncpy(retval.name, name->valuestring, 80);
    if(cJSON_IsString(message)) strncpy(retval.message, message->valuestring, 4096);
    if(cJSON_IsNumber(status)) retval.status = status->valueint;

    return retval;
}

// Deserialize Restriction
Restriction unmarshalRestriction(cJSON *body) {
    Restriction retval;
    retval.type[0] = 0;
    retval.value[0] = 0;
    cJSON *type, *value;

    type = cJSON_GetObjectItem(body, "type");
    value = cJSON_GetObjectItem(body, "value");

    if(cJSON_IsString(type)) strncpy(retval.type, type->valuestring, 80);
    if(cJSON_IsString(value)) strncpy(retval.value, value->valuestring, 512);

    return retval;
}

// Deserialize RestrictionResponse
RestrictionResponse unmarshalRestrictionResponse(cJSON *body) {
    RestrictionResponse retval;
    Restriction emptyRestriction;
    emptyRestriction.type[0] = 0;
    emptyRestriction.value[0] = 0;
    retval.restrictions[0] = emptyRestriction;
    int i = 0;

    cJSON *restrictions, *restriction;
    restrictions = cJSON_GetObjectItem(body, "restrictions");
    cJSON_ArrayForEach(restriction, restrictions) {
        retval.restrictions[i] = unmarshalRestriction(restriction);
        i++;
        if(i>100) break;
    }

    return retval;
}

// Deserialize General Response
GeneralResponse unmarshalGeneralResponse(cJSON *body) {
    GeneralResponse retval;
    retval.success = false;
    retval.description[0] = 0;
    cJSON *success, *description;

    success = cJSON_GetObjectItem(body, "success");
    description = cJSON_GetObjectItem(body, "description");

    if(cJSON_IsBool(success)) retval.success = success->valueint;
    if(cJSON_IsString(description)) strncpy(retval.description, description->valuestring, 512);

    return retval;
}

// Deserialize FilesInfo
FilesInfo unmarshalFilesInfo(cJSON *body) {
    FilesInfo retval;
    retval.recordCount = -1;
    retval.recordSize = -1;
    cJSON *recordCount, *recordSize;

    recordCount = cJSON_GetObjectItem(body, "recordCount");
    recordSize = cJSON_GetObjectItem(body, "recordSize");

    if(cJSON_IsNumber(recordCount)) retval.recordCount = recordCount->valueint;
    if(cJSON_IsNumber(recordSize))  retval.recordSize = recordSize->valueint;

    return retval;
}

// Parse Json from string
cJSON *ParseJson(char *body) {
    cJSON *jsonObj = cJSON_Parse(body);
    if (jsonObj == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
    }
    return jsonObj;
}

// Deserializers
FileResponse deserializeResponse(char *body) {
    cJSON *fileObject = ParseJson(body);
    FileResponse retval = unmarshalFileResponse(fileObject);
    cJSON_Delete(fileObject);
    return retval;
}

BucketResponse deserializeBucketResponse(char *body) {
    cJSON *bucketObject = ParseJson(body);
    BucketResponse retval = unmarshalBucket(bucketObject);
    cJSON_Delete(bucketObject);
    return retval;
}

AlbumResponse deserializeAlbumResponse(char *body) {
    cJSON *albumObject = ParseJson(body);
    AlbumResponse retval = unmarshalAlbumResponse(albumObject);
    cJSON_Delete(albumObject);
    return retval;
}

AlbumInfo deserializeAlbumInfo(char *body) {
    cJSON *albumObject = ParseJson(body);
    AlbumInfo retval = unmarshalAlbumInfo(albumObject);
    cJSON_Delete(albumObject);
    return retval;
}

RestrictionResponse deserializeRestrictionResponse(char *body) {
    cJSON *restObject = ParseJson(body);
    RestrictionResponse retval = unmarshalRestrictionResponse(restObject);
    cJSON_Delete(restObject);
    return retval;
}

FilesInfo deserializeFilesInfo(char *body) {
    cJSON *infoObject = ParseJson(body);
    FilesInfo retval = unmarshalFilesInfo(infoObject);
    cJSON_Delete(infoObject);
    return retval;
}

ErrorResponse deserializeErrorResponse(char *body) {
    cJSON *errorObject = ParseJson(body);
    ErrorResponse retval = unmarshalErrorResponse(errorObject);
    cJSON_Delete(errorObject);
    return retval;
}
#endif //WAIFUVAULT_C_DESERIALIZERS_H
