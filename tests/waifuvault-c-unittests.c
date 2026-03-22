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
    static char *response = "{\"url\":\"https://waifuvault.moe/f/something\", \"token\":\"test-token\", \"bucket\":\"test-bucket\", \"retentionPeriod\":100, \"options\":{\"protected\":false, \"hideFilename\":false, \"oneTimeDownload\":false}}";
    static MemoryStream contents;
    contents.memory = response;
    contents.size = strlen(response);
    dispatchMock.returns = CURLE_OK;
    dispatchMock.contents = &contents;
    dispatchMock.calls = 0;
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
    static char *response = "{\"url\":\"https://waifuvault.moe/f/something\", \"token\":\"test-token\", \"bucket\":\"test-bucket\", \"retentionPeriod\":100, \"options\":{\"protected\":false, \"hideFilename\":false, \"oneTimeDownload\":false}}";
    static MemoryStream contents;
    contents.memory = response;
    contents.size = strlen(response);
    dispatchMock.returns = CURLE_OK;
    dispatchMock.contents = &contents;
    dispatchMock.calls = 0;
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
    static char *response = "{\"url\":\"https://waifuvault.moe/f/something\", \"token\":\"test-token\", \"bucket\":\"test-bucket\", \"retentionPeriod\":\"10 minutes\", \"options\":{\"protected\":false, \"hideFilename\":false, \"oneTimeDownload\":false}}";
    static MemoryStream contents;
    contents.memory = response;
    contents.size = strlen(response);
    dispatchMock.returns = CURLE_OK;
    dispatchMock.contents = &contents;
    dispatchMock.calls = 0;

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

int main(void) {
    openCurl();
    clearRestrictions();
    testURLUpload();
    testFileUpload();
    testFileInfo();
    closeCurl();
    return 0;
}