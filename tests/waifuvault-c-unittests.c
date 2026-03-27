// Unit tests
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#define WAIFUVAULT_C_UNIT_TESTS
#include <string.h>

#include "waifuvault-c-mocks.h"
#include<unistd.h>
#include "../waifuvault-c-api.h"
#include "../waifuvault-c-models.h"

struct dispatchMock dispatchMock = {
    0, CURLE_OK, NULL, NULL, NULL, NULL, NULL, NULL
};

// Responses
static char *fileInfoOK = "{\"url\":\"https://waifuvault.moe/f/something\", \"token\":\"test-token\", \"bucket\":\"test-bucket\", \"retentionPeriod\":100, \"options\":{\"protected\":false, \"hideFilename\":false, \"oneTimeDownload\":false}}";
static char *fileInfoOKText = "{\"url\":\"https://waifuvault.moe/f/something\", \"token\":\"test-token\", \"bucket\":\"test-bucket\", \"retentionPeriod\":\"10 minutes\", \"options\":{\"protected\":false, \"hideFilename\":false, \"oneTimeDownload\":false}}";
static char *deleteTrue = "true";
static char *emptyBucket = "{\"token\":\"test-bucket\", \"files\":[]}";
static char *usedBucket = "{\"token\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"files\":[{\"bucket\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"retentionPeriod\":null,\"album\":null,\"token\":\"0dd4b9b5-1e7e-4852-bdc5-54a79feb07c9\",\"id\":21343,\"views\":13,\"url\":\"https://waifuvault.moe/f/d270ad3d-3992-4dec-9ddd-ee32c6f5706f/voice.zip\",\"options\":{\"hideFilename\":false,\"oneTimeDownload\":false,\"protected\":false}},{\"bucket\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"retentionPeriod\":20905635650,\"album\":{\"token\":\"b96413f7-2e34-4691-8f44-6b9fcf83ca7c\",\"publicToken\":\"ce8c7459-b26f-4844-b65a-4d1668308c8e\",\"name\":\"Something\",\"bucket\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"dateCreated\":1766428873426},\"token\":\"bb183720-58eb-44d6-9eff-d72536edf302\",\"id\":21084,\"views\":0,\"url\":\"https://waifuvault.moe/f/0b62bc0d-f0fc-471f-aacb-f9e35b0e6821/having%20an%20excited%20conversation%20over%20tea%20in%20a%20victorian%20setting%20s-1073058833.png\",\"options\":{\"hideFilename\":false,\"oneTimeDownload\":false,\"protected\":false}}],\"albums\":[{\"token\":\"b96413f7-2e34-4691-8f44-6b9fcf83ca7c\",\"publicToken\":\"ce8c7459-b26f-4844-b65a-4d1668308c8e\",\"name\":\"Something\",\"bucket\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"dateCreated\":1766428873426}]}";
static char *albumNew = "{\"token\": \"test-album\", \"bucketToken\":\"test-bucket\", \"publicToken\":null, \"name\":\"test-name\", \"files\":[]}";
static char *albumWithFiles = "{\"token\":\"b96413f7-2e34-4691-8f44-6b9fcf83ca7c\",\"bucketToken\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"publicToken\":\"ce8c7459-b26f-4844-b65a-4d1668308c8e\",\"name\":\"Something\",\"dateCreated\":1766428873426,\"files\":[{\"bucket\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"retentionPeriod\":20841380227,\"album\":null,\"token\":\"bb183720-58eb-44d6-9eff-d72536edf302\",\"id\":21084,\"views\":0,\"url\":\"https://waifuvault.moe/f/0b62bc0d-f0fc-471f-aacb-f9e35b0e6821/having%20an%20excited%20conversation%20over%20tea%20in%20a%20victorian%20setting%20s-1073058833.png\",\"options\":{\"hideFilename\":false,\"oneTimeDownload\":false,\"protected\":false}},{\"bucket\":\"56a62473-d3ef-48f9-baef-3628a3d23549\",\"retentionPeriod\":null,\"album\":null,\"token\":\"49cc14d8-c4da-410a-91f7-09848f1e8466\",\"id\":22185,\"views\":0,\"url\":\"https://waifuvault.moe/f/b9d1f463-5f41-49cb-980b-ca3043382634/1999.jpg\",\"options\":{\"hideFilename\":false,\"oneTimeDownload\":false,\"protected\":false}}]}";
static char *generalTrue = "{\"success\":true, \"description\":\"yes\"}";
static unsigned char fileReturn[4] = {0xba, 0xad, 0xf0, 0x0d};

void clearMocks() {
    dispatchMock.calls = 0;
    dispatchMock.returns = CURLE_OK;
    dispatchMock.contents = NULL;
    dispatchMock.targetUrl = NULL;
    dispatchMock.targetMethod = NULL;
    dispatchMock.fields = NULL;
    dispatchMock.formpost = NULL;
    dispatchMock.headers = NULL;
}

void testURLUpload() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = fileInfoOK;
    contents.size = strlen(fileInfoOK);
    dispatchMock.contents = &contents;
    FileUpload urlUpload = CreateFileUpload("https://waifuvault.moe/assets/custom/images/08.png","10m","",false,false);

    // When
    FileResponse uploadResponse = uploadFile(urlUpload);

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("https://waifuvault.moe/f/something",uploadResponse.url,strlen(uploadResponse.url)) == 0);
    assert(strncmp("test-token",uploadResponse.token,strlen(uploadResponse.token)) == 0);
    assert(uploadResponse.options.protected == false);
    assert(strncmp("100",uploadResponse.retentionPeriod,strlen(uploadResponse.retentionPeriod)) == 0);
    printf("URL Upload test passed\n");
}

void testFileUpload() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = fileInfoOK;
    contents.size = strlen(fileInfoOK);
    dispatchMock.contents = &contents;
    FileUpload fileUpload = CreateFileUpload("test.png","10m","",false,false);

    // When
    FileResponse uploadResponse = uploadFile(fileUpload);

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("https://waifuvault.moe/f/something",uploadResponse.url,strlen(uploadResponse.url)) == 0);
    assert(strncmp("test-token",uploadResponse.token,strlen(uploadResponse.token)) == 0);
    assert(uploadResponse.options.protected == false);
    assert(strncmp("100",uploadResponse.retentionPeriod,strlen(uploadResponse.retentionPeriod)) == 0);
    printf("File Upload test passed\n");
}

void testFileInfo() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = fileInfoOKText;
    contents.size = strlen(fileInfoOKText);
    dispatchMock.contents = &contents;

    // When
    FileResponse infoResponse = fileInfo("test-token", true);

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("https://waifuvault.moe/f/something",infoResponse.url,strlen(infoResponse.url)) == 0);
    assert(strncmp("test-token",infoResponse.token,strlen(infoResponse.token)) == 0);
    assert(infoResponse.options.protected == false);
    assert(strncmp("10 minutes",infoResponse.retentionPeriod,strlen(infoResponse.retentionPeriod)) == 0);
    printf("File Info test passed\n");
}

void testFileInfoNumeric() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = fileInfoOK;
    contents.size = strlen(fileInfoOK);
    dispatchMock.contents = &contents;

    // When
    FileResponse infoResponse = fileInfo("test-token", true);

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("https://waifuvault.moe/f/something",infoResponse.url,strlen(infoResponse.url)) == 0);
    assert(strncmp("test-token",infoResponse.token,strlen(infoResponse.token)) == 0);
    assert(infoResponse.options.protected == false);
    assert(strncmp("100",infoResponse.retentionPeriod,strlen(infoResponse.retentionPeriod)) == 0);
    printf("File Info Numeric test passed\n");
}

void testUpdateInfo() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = fileInfoOK;
    contents.size = strlen(fileInfoOK);
    dispatchMock.contents = &contents;

    // When
    FileResponse updateResponse = fileUpdate("test-token", "password", "previous", "exp", false);

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("https://waifuvault.moe/f/something",updateResponse.url,strlen(updateResponse.url)) == 0);
    assert(strncmp("PATCH", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("test-token",updateResponse.token,strlen(updateResponse.token)) == 0);
    assert(strncmp("{\"password\":\"password\",\"previousPassword\":\"previous\",\"customExpiry\":\"exp\",\"hideFilename\":false}",
        dispatchMock.fields, strlen(dispatchMock.fields)) == 0);
    printf("File Update test passed\n");
}

void testDelete() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = deleteTrue;
    contents.size = strlen(deleteTrue);
    dispatchMock.contents = &contents;

    // When
    bool deleteResponse = deleteFile("test-token");

    // Then
    assert(dispatchMock.calls == 1);
    assert(deleteResponse);
    assert(strncmp("DELETE", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/test-token", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    printf("Delete test passed\n");
}

void testDownload() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = (char*)&fileReturn;
    contents.size = 4;
    dispatchMock.contents = &contents;
    FileResponse target;
    strncpy(target.token,"test-token\0",11);
    strncpy(target.url,"https://waifuvault.moe/f/something\0",35);
    MemoryStream downloadResponse;

    // When
    getFile(target, &downloadResponse,"");

    // Then
    assert(dispatchMock.calls == 1);
    assert(downloadResponse.size == 4);
    assert(strncmp("GET", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/f/something", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(memcmp(contents.memory,downloadResponse.memory,downloadResponse.size) == 0);
    printf("Download test passed\n");
}

void testCreateBucket() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = emptyBucket;
    contents.size = strlen(emptyBucket);
    dispatchMock.contents = &contents;

    // When
    BucketResponse bresponse = createBucket();

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("GET", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/bucket/create", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    printf("Create Bucket test passed\n");
}

void testGetBucket() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = usedBucket;
    contents.size = strlen(usedBucket);
    dispatchMock.contents = &contents;

    // When
    BucketResponse bresponse = getBucket("test-token");

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("POST", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/bucket/get", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(strncmp("{\"bucket_token\":\"test-token\"}",dispatchMock.fields, strlen(dispatchMock.fields)) == 0);
    assert(bresponse.files.count == 2);
    assert(strncmp("0dd4b9b5-1e7e-4852-bdc5-54a79feb07c9", bresponse.files.items[0].token, strlen(bresponse.files.items[0].token)) == 0);
    assert(bresponse.albums.count == 1);
    assert(strncmp("b96413f7-2e34-4691-8f44-6b9fcf83ca7c", bresponse.albums.items[0].token, strlen(bresponse.albums.items[0].token)) == 0);
    printf("Get Bucket test passed\n");
}

void testDeleteBucket() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = deleteTrue;
    contents.size = strlen(deleteTrue);
    dispatchMock.contents = &contents;

    // When
    bool deleteBucketResponse = deleteBucket("test-token");

    // Then
    assert(dispatchMock.calls == 1);
    assert(deleteBucketResponse);
    assert(strncmp("DELETE", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/bucket/test-token", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    printf("Delete Bucket test passed\n");
}

void testCreateAlbum() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = albumNew;
    contents.size = strlen(albumNew);
    dispatchMock.contents = &contents;

    // When
    AlbumResponse albumResponse = createAlbum("test-bucket", "test-name");

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("POST", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/test-bucket", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(strncmp("test-name", albumResponse.name, strlen(albumResponse.name)) == 0);
    assert(strncmp("test-album", albumResponse.token, strlen(albumResponse.token)) == 0);
    printf("Create Album test passed\n");
}

void testDeleteAlbum() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = generalTrue;
    contents.size = strlen(generalTrue);
    dispatchMock.contents = &contents;

    // When
    bool deleteAlbumResponse = deleteAlbum("test-album", true);

    // Then
    assert(dispatchMock.calls == 1);
    assert(deleteAlbumResponse);
    assert(strncmp("DELETE", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/test-album?deleteFiles=true", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    printf("Delete Album test passed\n");
}

void testGetAlbum() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = albumWithFiles;
    contents.size = strlen(albumWithFiles);
    dispatchMock.contents = &contents;

    // When
    AlbumResponse albumResponse = getAlbum("test-token");

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("GET", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/test-token", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(strncmp("Something", albumResponse.name, strlen(albumResponse.name)) == 0);
    assert(strncmp("b96413f7-2e34-4691-8f44-6b9fcf83ca7c", albumResponse.token, strlen(albumResponse.token)) == 0);
    assert(strncmp("ce8c7459-b26f-4844-b65a-4d1668308c8e", albumResponse.publicToken, strlen(albumResponse.publicToken)) == 0);
    assert(strncmp("56a62473-d3ef-48f9-baef-3628a3d23549", albumResponse.bucketToken, strlen(albumResponse.bucketToken)) == 0);
    assert(albumResponse.files.count == 2);
    assert(strncmp("bb183720-58eb-44d6-9eff-d72536edf302", albumResponse.files.items[0].token, strlen(albumResponse.files.items[0].token)) == 0);
    assert(strncmp("49cc14d8-c4da-410a-91f7-09848f1e8466", albumResponse.files.items[1].token, strlen(albumResponse.files.items[1].token)) == 0);
    printf("Get Album test passed\n");
}

void testShareAlbum() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = generalTrue;
    contents.size = strlen(generalTrue);
    dispatchMock.contents = &contents;

    // When
    char *shareAlbumResponse = shareAlbum("test-album");

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("GET", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/share/test-album", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(strncmp("yes", shareAlbumResponse, strlen(shareAlbumResponse)) == 0);
    printf("Share Album test passed\n");
}

void testRevokeAlbum() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = generalTrue;
    contents.size = strlen(generalTrue);
    dispatchMock.contents = &contents;

    // When
    bool revokeAlbumResponse = revokeAlbum("test-album");

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("GET", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/revoke/test-album", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(revokeAlbumResponse);
    printf("Revoke Album test passed\n");
}

void testAssociateFiles() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = albumWithFiles;
    contents.size = strlen(albumWithFiles);
    dispatchMock.contents = &contents;
    char *files[2] = {"file-token-1", "file-token-2"};

    // When
    AlbumResponse albumResponse = associateFiles("album-token", files, 2);

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("POST", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/album-token/associate", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(strncmp("{\"fileTokens\":[\"file-token-1\",\"file-token-2\"]}",dispatchMock.fields, strlen(dispatchMock.fields)) == 0);
    assert(strncmp("Something", albumResponse.name, strlen(albumResponse.name)) == 0);
    assert(strncmp("b96413f7-2e34-4691-8f44-6b9fcf83ca7c", albumResponse.token, strlen(albumResponse.token)) == 0);
    assert(strncmp("ce8c7459-b26f-4844-b65a-4d1668308c8e", albumResponse.publicToken, strlen(albumResponse.publicToken)) == 0);
    assert(strncmp("56a62473-d3ef-48f9-baef-3628a3d23549", albumResponse.bucketToken, strlen(albumResponse.bucketToken)) == 0);
    assert(albumResponse.files.count == 2);
    assert(strncmp("bb183720-58eb-44d6-9eff-d72536edf302", albumResponse.files.items[0].token, strlen(albumResponse.files.items[0].token)) == 0);
    assert(strncmp("49cc14d8-c4da-410a-91f7-09848f1e8466", albumResponse.files.items[1].token, strlen(albumResponse.files.items[1].token)) == 0);
    printf("Associate Files test passed\n");
}

void testDisassociateFiles() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = albumWithFiles;
    contents.size = strlen(albumWithFiles);
    dispatchMock.contents = &contents;
    char *files[2] = {"file-token-1", "file-token-2"};

    // When
    AlbumResponse albumResponse = disassociateFiles("album-token", files, 2);

    // Then
    assert(dispatchMock.calls == 1);
    assert(strncmp("POST", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/album-token/disassociate", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(strncmp("{\"fileTokens\":[\"file-token-1\",\"file-token-2\"]}",dispatchMock.fields, strlen(dispatchMock.fields)) == 0);
    assert(strncmp("Something", albumResponse.name, strlen(albumResponse.name)) == 0);
    assert(strncmp("b96413f7-2e34-4691-8f44-6b9fcf83ca7c", albumResponse.token, strlen(albumResponse.token)) == 0);
    assert(strncmp("ce8c7459-b26f-4844-b65a-4d1668308c8e", albumResponse.publicToken, strlen(albumResponse.publicToken)) == 0);
    assert(strncmp("56a62473-d3ef-48f9-baef-3628a3d23549", albumResponse.bucketToken, strlen(albumResponse.bucketToken)) == 0);
    assert(albumResponse.files.count == 2);
    assert(strncmp("bb183720-58eb-44d6-9eff-d72536edf302", albumResponse.files.items[0].token, strlen(albumResponse.files.items[0].token)) == 0);
    assert(strncmp("49cc14d8-c4da-410a-91f7-09848f1e8466", albumResponse.files.items[1].token, strlen(albumResponse.files.items[1].token)) == 0);
    printf("Disassociate Files test passed\n");
}

void testDownloadAlbum() {
    // Given
    clearMocks();
    static MemoryStream contents;
    contents.memory = (char*)&fileReturn;
    contents.size = 4;
    dispatchMock.contents = &contents;
    int files[2] = {6,7};
    MemoryStream downloadResponse;

    // When
    downloadAlbum("album-token",files,2,&downloadResponse);

    // Then
    assert(dispatchMock.calls == 1);
    assert(downloadResponse.size == 4);
    assert(strncmp("POST", dispatchMock.targetMethod, strlen(dispatchMock.targetMethod)) == 0);
    assert(strncmp("https://waifuvault.moe/rest/album/download/album-token", dispatchMock.targetUrl, strlen(dispatchMock.targetUrl)) == 0);
    assert(strncmp("[6,7]",dispatchMock.fields, strlen(dispatchMock.fields)) == 0);
    assert(memcmp(contents.memory,downloadResponse.memory,downloadResponse.size) == 0);
    printf("DownloadAlbum test passed\n");
}

void testGetRestrictions() {
    // To be implemented
}

void testGetFileStats() {
    // To be implemented
}

int main(void) {
    openCurl();
    clearRestrictions();
    testURLUpload();
    testFileUpload();
    testFileInfo();
    testFileInfoNumeric();
    testUpdateInfo();
    testDelete();
    testDownload();
    testCreateBucket();
    testGetBucket();
    testDeleteBucket();
    testCreateAlbum();
    testDeleteAlbum();
    testGetAlbum();
    testShareAlbum();
    testRevokeAlbum();
    testAssociateFiles();
    testDisassociateFiles();
    testDownloadAlbum();
    closeCurl();
    return 0;
}