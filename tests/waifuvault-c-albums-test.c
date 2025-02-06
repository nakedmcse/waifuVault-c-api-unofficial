// Test program for albums api
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include "../waifuvault-c-api.h"
#include "../waifuvault-c-models.h"

int main(void) {
    FileResponse uploadResponse;
    BucketResponse bucketCreate, bucketGet;
    FileUpload fileData;
    ErrorResponse *err;
    bool deleteResponse;

    openCurl();

    // Set Alternate BASEURL
    setAltBaseURL("https://waifuvault.walker.moe/rest");
    printf("--SET ALT BASE URL COMPLETED--\n");

    // Create bucket
    bucketCreate = createBucket();
    printf("--CREATE BUCKET COMPLETED--\n");
    printf("Bucket token: %s\n\n", bucketCreate.token);
    sleep(1);

    // Upload two files
    fileData = CreateBucketFileUpload("~/Downloads/GothAlya.jpeg",bucketCreate.token,"","",false,false);
    uploadResponse = uploadFile(fileData);
    printf("--UPLOAD ONE COMPLETED--\n");
    printf("URL: %s\n", uploadResponse.url);
    printf("Token: %s\n", uploadResponse.token);
    printf("Retention: %s\n\n", uploadResponse.retentionPeriod);
    sleep(1);

    uploadResponse = fileInfo(uploadResponse.token, false);
    printf("--UPLOAD ONE INFO COMPLETED--\n");
    printf("URL: %s\n", uploadResponse.url);
    printf("Token: %s\n", uploadResponse.token);
    printf("Retention: %s\n\n", uploadResponse.retentionPeriod);
    sleep(1);

    fileData = CreateBucketFileUpload("~/Downloads/vic_laptop.jpg",bucketCreate.token,"10m","",false,false);
    uploadResponse = uploadFile(fileData);
    printf("--UPLOAD TWO COMPLETED--\n");
    printf("URL: %s\n", uploadResponse.url);
    printf("Token: %s\n", uploadResponse.token);
    printf("Retention: %s\n\n", uploadResponse.retentionPeriod);
    sleep(1);

    // Create Album

    // Associate Files

    // Share Album

    // Revoke Album

    // Disassociate Files

    // Delete Album

    // Delete Bucket

    return 0;
}
