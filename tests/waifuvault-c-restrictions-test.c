// Test program for buckets api
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include "../waifuvault-c-api.h"
#include "../waifuvault-c-models.h"

int main(void) {
    RestrictionResponse restrictions;
    FileResponse uploadResponse;
    FileUpload fileData;
    ErrorResponse *err;

    openCurl();

    // Get Restrictions
    restrictions = getRestrictions();
    printf("--GET RESTRICTIONS COMPLETED--\n");
    printf("Restriction 1: %s, %s\n", restrictions.restrictions[0].type, restrictions.restrictions[0].value);
    printf("Restriction 2: %s, %s\n\n", restrictions.restrictions[1].type, restrictions.restrictions[1].value);
    sleep(1);

    // Upload bad file
    fileData = CreateFileUpload("~/Dropbox/Public/filebundler.exe","10m","",false,false);
    uploadResponse = uploadFile(fileData);
    printf("--UPLOAD BAD COMPLETED--\n");
    err = getError();
    if(err) {
        printf("Error Status: %d\n", err->status);
        printf("Error Name: %s\n", err->name);
        printf("Error Message: %s\n\n", err->message);
        clearError();
    } else {
        printf("URL: %s\n", uploadResponse.url);
        printf("Token: %s\n\n", uploadResponse.token);
    }
    sleep(1);

    // Upload good file
    fileData = CreateFileUpload("~/Downloads/rory2.jpg","10m","",false,false);
    uploadResponse = uploadFile(fileData);
    printf("--UPLOAD GOOD COMPLETED--\n");
    err = getError();
    if(err) {
        printf("Error Status: %d\n", err->status);
        printf("Error Name: %s\n", err->name);
        printf("Error Message: %s\n\n", err->message);
        clearError();
    } else {
        printf("URL: %s\n", uploadResponse.url);
        printf("Token: %s\n\n", uploadResponse.token);
    }
    sleep(1);

    // Clear Restrictions
    restrictions = clearRestrictions();
    printf("--CLEAR RESTRICTIONS COMPLETED--\n");
    printf("Restriction 1: %s, %s\n", restrictions.restrictions[0].type, restrictions.restrictions[0].value);
    printf("Restriction 2: %s, %s\n", restrictions.restrictions[1].type, restrictions.restrictions[1].value);

    return 0;
}