// Test program for api
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include "../waifuvault-c-api.h"

int main(void) {
    FileResponse infoResponse, uploadResponse, updateResponse;
    FileUpload urlUpload;
    MemoryStream download;
    bool deleteResponse;
    ErrorResponse *err;
    FILE *file;
    char *buffer;
    long fileLen;

    openCurl();

    // Set Alternate BASEURL
    setAltBaseURL("https://waifuvault.walker.moe/rest");
    printf("--SET ALT BASE URL COMPLETED--\n");

    //urlUpload = CreateFileUpload("https://waifuvault.moe/assets/custom/images/08.png","10m","",false,false);
    urlUpload = CreateFileUpload("~/Downloads/rider3.png","10m","dangerWaifu",false,false);
    /*
    file = fopen("/Users/walker/Documents/Code/waifuVault-c-api-unofficial/tests/RoryMercury.png", "rb");
    if (!file) {
        fprintf(stderr, "failed to open file");
        exit(1);
    }

    fseek(file, 0, SEEK_END);
    fileLen = ftell(file);
    rewind(file);
    buffer = malloc(fileLen);
    if (!buffer) {
        fprintf(stderr,"Memory allocation failed");
        fclose(file);
        exit(1);
    }

    fread(buffer, 1, fileLen, file);
    if (ferror(file)) {
        fprintf(stderr,"File read failed");
        fclose(file);
        exit(1);
    }

    urlUpload = CreateBufferUpload(buffer,fileLen,"RoryMercury.png","10m","",false,false);
    */
    uploadResponse = uploadFile(urlUpload);
    //free(buffer);
    //fclose(file);
    printf("--UPLOAD COMPLETED--\n");
    printf("URL: %s\n", uploadResponse.url);
    printf("Token: %s\n\n", uploadResponse.token);
    sleep(1);

    infoResponse = fileInfo(uploadResponse.token,true);
    printf("--INFO COMPLETED--\n");
    printf("URL: %s\n", infoResponse.url);
    printf("Retention: %s\n", infoResponse.retentionPeriod);
    printf("Hidden Filename: %s\n", infoResponse.options.hasFilename ? "True" : "False");
    printf("One Time Download: %s\n", infoResponse.options.oneTimeDownload ? "True" : "False");
    printf("Encrypted: %s\n\n", infoResponse.options.protected ? "True" : "False");
    sleep(1);

    infoResponse = fileInfo("bad-token",true);
    printf("--BAD TOKEN COMPLETED--\n");
    err = getError();
    if(err) {
        printf("Error Status: %d\n", err->status);
        printf("Error Name: %s\n", err->name);
        printf("Error Message: %s\n\n", err->message);
        free(err);
    }
    sleep(1);

    updateResponse = fileUpdate(uploadResponse.token, "dangerWaifu2", "dangerWaifu", "", false);
    printf("--UPDATE COMPLETED--\n");
    printf("URL: %s\n", updateResponse.url);
    printf("Retention: %s\n", updateResponse.retentionPeriod);
    printf("Encrypted: %s\n\n", updateResponse.options.protected ? "True" : "False");
    sleep(1);

    getFile(uploadResponse, &download, "dangerWaifu2");
    printf("--DOWNLOAD COMPLETED--\n");
    printf("Download size: %zu\n\n", download.size);
    free(download.memory);
    sleep(1);

    deleteResponse = deleteFile(uploadResponse.token);
    printf("--DELETE COMPLETED--\n");
    printf("Return: %s\n\n", deleteResponse ? "True" : "False");

    closeCurl();
}