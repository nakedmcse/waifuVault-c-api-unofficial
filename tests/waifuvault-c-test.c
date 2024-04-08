// Test program for api
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include "../waifuvault-c-api.h"
#include "../waifuvault-c-models.h"

int main(void) {
    FileResponse infoResponse, uploadResponse;
    FileUpload urlUpload;
    bool deleteResponse;

    openCurl();

    urlUpload = CreateFileUpload("https://waifuvault.moe/assets/custom/images/08.png","10m","",false,false);
    uploadResponse = uploadFile(urlUpload);
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

    deleteResponse = deleteFile(uploadResponse.token);
    printf("--DELETE COMPLETED--\n");
    printf("Return: %s\n\n", deleteResponse ? "True" : "False");

    closeCurl();
}