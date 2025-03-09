// Test program for albums api
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include "../waifuvault-c-api.h"
#include "../waifuvault-c-models.h"

int main(void) {
    FileResponse uploadResponse1, uploadResponse2;
    BucketResponse bucketCreate, bucketGet;
    AlbumResponse albumCreate, albumAssoc, albumDis;
    FileUpload fileData;
    ErrorResponse *err;
    char *shareResp;
    bool deleteResponse, revokeResp;

    openCurl();

    // Set Alternate BASEURL
    setAltBaseURL("https://waifuvault.walker.moe/rest");
    //setAltBaseURL("http://localhost:8081/rest");
    printf("--SET ALT BASE URL COMPLETED--\n");

    // Create bucket
    bucketCreate = createBucket();
    printf("--CREATE BUCKET COMPLETED--\n");
    printf("Bucket token: %s\n\n", bucketCreate.token);
    sleep(1);

    // Upload two files
    fileData = CreateBucketFileUpload("~/Downloads/GothAlya.jpeg",bucketCreate.token,"","",false,false);
    uploadResponse1 = uploadFile(fileData);
    printf("--UPLOAD ONE COMPLETED--\n");
    printf("URL: %s\n", uploadResponse1.url);
    printf("Token: %s\n", uploadResponse1.token);
    printf("Retention: %s\n\n", uploadResponse1.retentionPeriod);
    sleep(1);

    uploadResponse1 = fileInfo(uploadResponse1.token, false);
    printf("--UPLOAD ONE INFO COMPLETED--\n");
    printf("URL: %s\n", uploadResponse1.url);
    printf("Token: %s\n", uploadResponse1.token);
    printf("Retention: %s\n\n", uploadResponse1.retentionPeriod);
    sleep(1);

    fileData = CreateBucketFileUpload("~/Downloads/vic_laptop.jpg",bucketCreate.token,"10m","",false,false);
    uploadResponse2 = uploadFile(fileData);
    printf("--UPLOAD TWO COMPLETED--\n");
    printf("URL: %s\n", uploadResponse2.url);
    printf("Token: %s\n", uploadResponse2.token);
    printf("Retention: %s\n\n", uploadResponse2.retentionPeriod);
    sleep(1);

    // Create Album
    albumCreate = createAlbum(bucketCreate.token, "test-album");
    printf("--CREATE ALBUM COMPLETED--\n");
    printf("Album token: %s\n\n", albumCreate.token);
    sleep(1);

    // Associate Files
    char *files[2];
    files[0] = uploadResponse1.token;
    files[1] = uploadResponse2.token;
    albumAssoc = associateFiles(albumCreate.token, files, 2);
    printf("--ALBUM FILES ASSOCIATE COMPLETED--\n");
    printf("File token 1: %s\nFile token 2: %s\n\n", albumAssoc.files.items[0].token, albumAssoc.files.items[1].token);
    sleep(1);

    // Share Album
    shareResp = shareAlbum(albumCreate.token);
    printf("--SHARE ALBUM COMPLETED--\n");
    printf("URL: %s\n\n", shareResp);
    sleep(1);

    // Revoke Album
    revokeResp = revokeAlbum(albumCreate.token);
    printf("--REVOKE ALBUM COMPLETED--\n");
    printf("Completed: %s\n\n", revokeResp ? "true" : "false");
    sleep(1);

    // Disassociate Files
    albumDis = disassociateFiles(albumCreate.token, files, 2);
    printf("--ALBUM FILES DISASSOCIATE COMPLETED--\n");
    printf("Files count: %lu\n", albumDis.files.count);
    sleep(1);

    // Delete Album
    deleteResponse = deleteAlbum(albumCreate.token, false);
    printf("--DELETE ALBUM COMPLETED--\n");
    printf("Completed: %s\n\n", deleteResponse ? "true" : "false");
    sleep(1);

    // Delete Bucket
    deleteResponse = deleteBucket(bucketCreate.token);
    printf("--DELETE BUCKET COMPLETED--\n");
    printf("Completed: %s\n\n", deleteResponse ? "true" : "false");
    sleep(1);

    return 0;
}
