// Test program for buckets api
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

    // Create bucket
    bucketCreate = createBucket();
    printf("--CREATE BUCKET COMPLETED--\n");
    printf("Bucket token: %s\n\n", bucketCreate.token);
    sleep(1);

    // Upload two files
    fileData = CreateBucketFileUpload("~/Downloads/rory2.jpg",bucketCreate.token,"10m","",false,false);
    uploadResponse = uploadFile(fileData);
    printf("--UPLOAD ONE COMPLETED--\n");
    printf("URL: %s\n", uploadResponse.url);
    printf("Token: %s\n\n", uploadResponse.token);
    sleep(1);

    fileData = CreateBucketFileUpload("~/Downloads/vic_laptop.jpg",bucketCreate.token,"10m","",false,false);
    uploadResponse = uploadFile(fileData);
    printf("--UPLOAD TWO COMPLETED--\n");
    printf("URL: %s\n", uploadResponse.url);
    printf("Token: %s\n\n", uploadResponse.token);
    sleep(1);

    // Get bucket contents
    bucketGet = getBucket(bucketCreate.token);
    err = getError();
    if(err) {
        printf("Error Status: %d\n", err->status);
        printf("Error Name: %s\n", err->name);
        printf("Error Message: %s\n\n", err->message);
        free(err);
    }
    printf("--GET BUCKET COMPLETED--\n");
    printf("Bucket token: %s\n", bucketGet.token);
    printf("File: %s\n", bucketGet.files[0].url);
    printf("Token: %s\n", bucketGet.files[0].token);
    printf("File: %s\n", bucketGet.files[1].url);
    printf("Token: %s\n\n", bucketGet.files[1].token);
    sleep(1);

    // Delete bucket
    deleteResponse = deleteBucket(bucketCreate.token);
    printf("--DELETE BUCKET COMPLETED--\n");
    printf("Return: %s\n\n", deleteResponse ? "True" : "False");

    closeCurl();
    return 0;
}