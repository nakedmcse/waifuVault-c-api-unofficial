# waifuvault-C-api-unofficial

![tests](https://github.com/nakedmcse/waifuVault-c-api-unofficial/actions/workflows/build.yml/badge.svg)
[![GitHub issues](https://img.shields.io/github/issues/nakedmcse/waifuvault-c-api-unofficial.png)](https://github.com/nakedmcse/waifuvault-c-api-unofficial/issues)
[![last-commit](https://img.shields.io/github/last-commit/nakedmcse/waifuvault-c-api-unofficial)](https://github.com/nakedmcse/waifuvault-c-api-unofficial/commits/master)

This contains the unofficial API bindings for uploading, deleting and obtaining files
with [waifuvault.moe](https://waifuvault.moe/). Contains a full up to date API for interacting with the service.

This is unofficial and as such will not be supported officially.  Use it at your own risk.  Updates to keep it comparable to the official 
SDKs will be on a best effort basis only.

## Installation

The SDK uses the microjson library for json parsing, as well as itself.  First build both .o files using the build script in the root directory
of the repo.

```sh
make sdk
```

This will give you mjson.o and waifuvault-c-api.o.  You will need both .o files and the header files for both the models and the api.  These get added as include lines at the top.

```c
#include "waifuvault-c-api.h"
#include "waifuvault-c-models.h"
```

Your code finally needs to be linked against mjson, waifuvault-c-api and curl.

```sh
gcc -o your-code your-code.c mjson.o waifuvault-c-api.o -lcurl
```

## Error Handling

After each call to the SDK, you need to use a call to getError to check for any errors during the call.  If it returns NULL, then all was OK. 
If there was an error, then you must consume the error with clearError.

```c
FileResponse infoResponse
ErrorResponse *err;

infoResponse = fileInfo("bad-token",true);

err = getError();
if(err) {
    printf("Error Status: %d\n", err->status);
    printf("Error Name: %s\n", err->name);
    printf("Error Message: %s\n\n", err->message);
    clearError(err)
    exit(1);
}
```

## Usage

This API contains 11 interactions:

1. [Upload File](#upload-file)
2. [Get File Info](#get-file-info)
3. [Update File Info](#update-file-info)
4. [Delete File](#delete-file)
5. [Get File](#get-file)
6. [Create Bucket](#create-bucket)
7. [Delete Bucket](#delete-bucket)
8. [Get Bucket](#get-bucket)
9. [Get Restrictions](#get-restrictions)
10. [Clear Restrictions](#clear-restrictions)
11. [Set Alternate Base URL](#set-alt-baseurl)

You need to include the header files in your code for the package:

```c
#include "waifuvault-c-api.h"
#include "waifuvault-c-models.h"
```

You also need to handle opening and closing a CURL session.  At the top of your code put:

```c
openCurl();
```

And at the bottom:

```c
closeCurl();
```

### Upload File<a id="upload-file"></a>

To Upload a file, use the `uploadFile` function. This function takes the following options as an object:

| Option            | Type         | Description                                                     | Required       | Extra info                       |
|-------------------|--------------|-----------------------------------------------------------------|----------------|----------------------------------|
| `filename`        | `string `    | The path to the file to upload                                  | true if File   | File path                        |
| `url`             | `string`     | The URL of the file to target                                   | true if URL    | Filename with extension          |
| `buffer`          | `byte array` | Byte array containing file to upload                            | true if buffer | Needs filename set also          |
| `bucketToken`     | `string`     | A bucket token to upload to                                     | false          | Use `CreateBucketFileUpload`     |
| `expires`         | `string`     | A string containing a number and a unit (1d = 1day)             | false          | Valid units are `m`, `h` and `d` |
| `hideFilename`    | `boolean`    | If true, then the uploaded filename won't appear in the URL     | false          | Defaults to `false`              |
| `password`        | `string`     | If set, then the uploaded file will be encrypted                | false          |                                  |
| `oneTimeDownload` | `boolean`    | if supplied, the file will be deleted as soon as it is accessed | false          |                                  |

> **NOTE:** If you use `getRestrictions` then server restrictions are checked by the SDK client side *before* upload, and will raise an error if they are violated

Using a URL:

```c
FileResponse uploadResponse;
FileUpload urlUpload;

urlUpload = CreateFileUpload("https://waifuvault.moe/assets/custom/images/08.png","10m","",false,false);
uploadResponse = uploadFile(urlUpload);

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
```

Using a file path:

```c
FileResponse uploadResponse;
FileUpload fileUpload;

fileUpload = CreateFileUpload("./acoolfile.png","10m","",false,false);
uploadResponse = uploadFile(fileUpload);

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
```

Using a file path to a bucket:

```c
FileResponse uploadResponse;
FileUpload fileUpload;

fileUpload = CreateBucketFileUpload("./acoolfile.png","some-bucket-token","10m","",false,false);
uploadResponse = uploadFile(fileUpload);

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
```

Using a buffer:

```c
FileResponse uploadResponse;
FileUpload bufferUpload;
FILE *file;
char *buffer;
long fileLen;

file = fopen("./acoolfile.png", "rb");
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

bufferUpload = CreateBufferUpload(buffer,fileLen,"acoolfile.png","10m","",false,false);
uploadResponse = uploadFile(bufferUpload);
free(buffer);
fclose(file);

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
```

### Get File Info<a id="get-file-info"></a>

If you have a token from your upload. Then you can get file info. This results in the following info:

* token
* url
* protected
* retentionPeriod

Use the `fileInfo` function. This function takes the following options as parameters:

| Option      | Type      | Description                                                        | Required | Extra info        |
|-------------|-----------|--------------------------------------------------------------------|----------|-------------------|
| `token`     | `string`  | The token of the upload                                            | true     |                   |
| `formatted` | `boolean` | If you want the `retentionPeriod` to be human-readable or an epoch | false    | defaults to false |

Epoch timestamp:

```c
FileResponse infoResponse;

infoResponse = fileInfo(token, false);

printf("URL: %s\n", infoResponse.url);
printf("Retention: %s\n", infoResponse.retentionPeriod);
printf("Hidden Filename: %s\n", infoResponse.options.hasFilename ? "True" : "False");
printf("One Time Download: %s\n", infoResponse.options.oneTimeDownload ? "True" : "False");
printf("Encrypted: %s\n\n", infoResponse.options.protected ? "True" : "False");
```

Human-readable timestamp:

```c
FileResponse infoResponse;

infoResponse = fileInfo(token, true);

printf("URL: %s\n", infoResponse.url);
printf("Retention: %s\n", infoResponse.retentionPeriod);
printf("Hidden Filename: %s\n", infoResponse.options.hasFilename ? "True" : "False");
printf("One Time Download: %s\n", infoResponse.options.oneTimeDownload ? "True" : "False");
printf("Encrypted: %s\n\n", infoResponse.options.protected ? "True" : "False");
```

### Update File Info<a id="update-file-info"></a>

If you have a token from your upload, then you can update the information for the file.  You can change the password or remove it, 
you can set custom expiry time or remove it, and finally you can choose whether the filename is hidden.

Use the `fileUpdate` function. This function takes the following options as parameters:

| Option              | Type     | Description                                             | Required | Extra info                                  |
|---------------------|----------|---------------------------------------------------------|----------|---------------------------------------------|
| `token`             | `string` | The token of the upload                                 | true     |                                             |
| `password`          | `string` | The current password of the file                        | false    | Set to empty string to remove password      |
| `previousPassword`  | `string` | The previous password of the file, if changing password | false    |                                             |
| `customExpiry`      | `string` | Custom expiry in the same form as upload command        | false    | Set to empty string to remove custom expiry |
| `hideFilename`      | `bool`   | Sets whether the filename is visible in the URL or not  | false    |                                             |

```c
FileResponse updateResponse;

updateResponse = fileUpdate(uploadResponse.token, password, previousPassword, customExpiry, false);

printf("URL: %s\n", updateResponse.url);
printf("Retention: %s\n", updateResponse.retentionPeriod);
printf("Encrypted: %s\n\n", updateResponse.options.protected ? "True" : "False");
```

### Delete File<a id="delete-file"></a>

To delete a file, you must supply your token to the `deletefile` function.

This function takes the following options as parameters:

| Option  | Type     | Description                              | Required | Extra info |
|---------|----------|------------------------------------------|----------|------------|
| `token` | `string` | The token of the file you wish to delete | true     |            |

Standard delete:

```c
bool deleteResponse;

deleteResponse = deleteFile(uploadResponse.token);

printf("Return: %s\n\n", deleteResponse ? "True" : "False");
```

### Get File<a id="get-file"></a>

This lib also supports obtaining a file from the API as a Buffer by supplying either the token or the unique identifier
of the file (epoch/filename).

Use the `getFile` function. This function takes the following options an object:

| Option     | Type     | Description                                | Required                           | Extra info                                      |
|------------|----------|--------------------------------------------|------------------------------------|-------------------------------------------------|
| `token`    | `string` | The token of the file you want to download | true only if `filename` is not set | if `filename` is set, then this can not be used |
| `url`      | `string` | The URL of the file                        | true only if `token` is not set    | if `token` is set, then this can not be used    |
| `download` | `pointer` | Pointer to a MemoryStream object to hold download | true         | Passed as a parameter on the function call      |
| `password` | `string` | The password for the file                  | true if file is encrypted          | Passed as a parameter on the function call      |

> **Important!** The Unique identifier filename is the epoch/filename only if the file uploaded did not have a hidden
> filename, if it did, then it's just the epoch.
> For example: `1710111505084/08.png` is the Unique identifier for a standard upload of a file called `08.png`, if this
> was uploaded with hidden filename, then it would be `1710111505084.png`

Obtain an encrypted file

```c
MemoryStream download;
FileResponse fileresp;

fileresp.token = "your-token";
// or fileresp.url = "your-url";
getFile(fileResp, &download, "dangerWaifu");

printf("Download size: %zu\n\n", download.size);
// Do something with download.memory
free(download.memory);
```

### Create Bucket<a id="create-bucket"></a>

Buckets are virtual collections that are linked to your IP and a token. When you create a bucket, you will receive a bucket token that you can use in Get Bucket to get all the files in that bucket

> **NOTE:** Only one bucket is allowed per client IP address, if you call it more than once, it will return the same bucket token

To create a bucket, use the `createBucket` function. This function does not take any arguments.

```c
BucketResponse bucketCreate = createBucket();

printf("Bucket token: %s\n", bucketCreate.token);
```

### Delete Bucket<a id="delete-bucket"></a>

Deleting a bucket will delete the bucket and all the files it contains.

> **IMPORTANT:**  All contained files will be **DELETED** along with the Bucket!

To delete a bucket, you must call the `deleteBucket` function with the following options as parameters:

| Option      | Type      | Description                       | Required | Extra info        |
|-------------|-----------|-----------------------------------|----------|-------------------|
| `token`     | `string`  | The token of the bucket to delete | true     |                   |

> **NOTE:** `deleteBucket` will only ever either return `true` or throw an exception if the token is invalid

```c
bool deleteResponse = deleteBucket("some-bucket-token");

printf("Return: %s\n", deleteResponse ? "True" : "False");
```

### Get Bucket<a id="get-bucket"></a>

To get the list of files contained in a bucket, you use the `getBucket` function and supply the token.
This function takes the following options as parameters:

| Option      | Type      | Description             | Required | Extra info        |
|-------------|-----------|-------------------------|----------|-------------------|
| `token`     | `string`  | The token of the bucket | true     |                   |

This will respond with the bucket and all the files the bucket contains.

```c
BucketResponse bucketGet = getBucket(bucketCreate.token);

printf("Bucket token: %s\n", bucketGet.token);
printf("File: %s\n", bucketGet.files[0].url);
printf("Token: %s\n", bucketGet.files[0].token);
printf("File: %s\n", bucketGet.files[1].url);
printf("Token: %s\n, bucketGet.files[1].token);
```

### Get Restrictions<a id="get-restrictions"></a>

To get the list of restrictions applied to the server, you use the `getRestrictions` function.

This will respond with an array of name, value entries describing the restrictions applied to the server.

> **NOTE:** This loads the server restrictions into the SDK and they will be validated client side before attempting to send

```c
RestrictionResponse restrictions;

// Get Restrictions
restrictions = getRestrictions();
printf("--GET RESTRICTIONS COMPLETED--\n");
printf("Restriction 1: %s, %s\n", restrictions.restrictions[0].type, restrictions.restrictions[0].value);
printf("Restriction 2: %s, %s\n", restrictions.restrictions[1].type, restrictions.restrictions[1].value);
```

### Clear Restrictions<a id="clear-restrictions"></a>

To clear the loaded restrictions in the SDK, you use the `clearRestrictions` function.

This will remove the loaded restrictions and the SDK will no longer validate client side.

```c
RestrictionResponse restrictions;

// Clear Restrictions
restrictions = clearRestrictions();
printf("--CLEAR RESTRICTIONS COMPLETED--\n");
printf("Restriction 1: %s, %s\n", restrictions.restrictions[0].type, restrictions.restrictions[0].value);
printf("Restriction 2: %s, %s\n", restrictions.restrictions[1].type, restrictions.restrictions[1].value);
```

### Set Alternate Base URL<a id="set-alt-baseurl"></a>

To set a custom base URL in the SDK, you use the `setAltBaseURL` function.

This will change the base URL used for all functions within the SDK.

```c
// Set Alternate BASEURL
setAltBaseURL("https://waifuvault.walker.moe/rest");
printf("--SET ALT BASE URL COMPLETED--\n");
```