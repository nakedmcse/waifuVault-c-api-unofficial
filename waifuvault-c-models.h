// Waifuvault C SDK Models
#include<stdbool.h>
#include<stdlib.h>

#ifndef WAIFUVAULT_C_MODELS
#define WAIFUVAULT_C_MODELS

// FileUpload
typedef struct FileUpload {
    char filename[512];
    char url[4096];
	char bucketToken[80];
    void *buffer;
    long bufferSize;
    char expires[10];
    char password[512];
    bool hideFilename;
    bool oneTimeDownload;
} FileUpload;

// FilesInfo
typedef struct FilesInfo {
    int recordCount;
    int recordSize;
} FilesInfo;

// FileOptions
typedef struct FileOptions {
    bool hasFilename;
    bool oneTimeDownload;
    bool protected;
} FileOptions;

// AlbumInfo
typedef struct AlbumInfo {
    char token[80];
    char publicToken[80];
    char name[120];
    char bucket[80];
    unsigned long dateCreated;
} AlbumInfo;

// FileResponse
typedef struct FileResponse {
    char token[80];
	char bucket[80];
    char url[4096];
    char retentionPeriod[80];
    int views;
    int id;
    AlbumInfo album;
    FileOptions options;
} FileResponse;

// AlbumResponse
typedef struct AlbumResponse {
    char token[80];
    char bucketToken[80];
    char publicToken[80];
    char name[120];
    FileResponse files[256];
    unsigned long dateCreated;
} AlbumResponse;

// BucketResponse
typedef struct BucketResponse {
    char token[80];
    FileResponse files[256];
    AlbumInfo albums[256];
} BucketResponse;

// ErrorResponse
typedef struct ErrorResponse {
    char name[80];
    int status;
    char message[4096];
} ErrorResponse;

// GeneralResponse
typedef struct GeneralResponse {
    bool success;
    char description[512];
} GeneralResponse;

// Restriction
typedef struct Restriction {
    char type[80];
    char value[512];
} Restriction;

// Restriction Response
typedef struct RestrictionResponse {
    Restriction restrictions[100];
} RestrictionResponse;

// MIME Lookup table
typedef struct MimeMap {
    char *ext;
    char *mime;
} MimeMap;

// List from https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
static MimeMap mimes[] = {
    {".aac", "audio/aac"},
    {".abw", "application/x-abiword"},
    {".arc", "application/x-freearc"},
    {".avi", "video/x-msvideo"},
    {".azw", "application/vnd.amazon.ebook"},
    {".bin", "application/octet-stream"},
    {".bmp", "image/bmp"},
    {".bz", "application/x-bzip"},
    {".bz2", "application/x-bzip2"},
    {".csh", "application/x-csh"},
    {".css", "text/css"},
    {".csv", "text/csv"},
    {".doc", "application/msword"},
    {".docx", "application/vnd.openxmlformats-officedocument.wordprocessingml.document"},
    {".eot", "application/vnd.ms-fontobject"},
    {".epub", "application/epub+zip"},
    {".exe", "application/x-dosexec"},
    {".gz", "application/gzip"},
    {".gif", "image/gif"},
    {".htm", "text/html"},
    {".html", "text/html"},
    {".ico", "image/vnd.microsoft.icon"},
    {".ics", "text/calendar"},
    {".jar", "application/java-archive"},
    {".jpeg", "image/jpeg"},
    {".jpg", "image/jpeg"},
    {".js", "text/javascript"},
    {".json", "application/json"},
    {".jsonld", "application/ld+json"},
    {".mid", "audio/midi"},
    {".midi", "audio/midi"},
    {".mjs", "text/javascript"},
    {".mp3", "audio/mpeg"},
    {".mp4", "video/mp4"},
    {".mpeg", "video/mpeg"},
    {".mpkg", "application/vnd.apple.installer+xml"},
    {".odp", "application/vnd.oasis.opendocument.presentation"},
    {".ods", "application/vnd.oasis.opendocument.spreadsheet"},
    {".odt", "application/vnd.oasis.opendocument.text"},
    {".oga", "audio/ogg"},
    {".ogv", "video/ogg"},
    {".ogx", "application/ogg"},
    {".opus", "audio/opus"},
    {".otf", "font/otf"},
    {".png", "image/png"},
    {".pdf", "application/pdf"},
    {".php", "application/x-httpd-php"},
    {".ppt", "application/vnd.ms-powerpoint"},
    {".pptx", "application/vnd.openxmlformats-officedocument.presentationml.presentation"},
    {".rar", "application/vnd.rar"},
    {".rtf", "application/rtf"},
    {".sh", "application/x-sh"},
    {".svg", "image/svg+xml"},
    {".swf", "application/x-shockwave-flash"},
    {".tar", "application/x-tar"},
    {".tif", "image/tiff"},
    {".tiff", "image/tiff"},
    {".ts", "video/mp2t"},
    {".ttf", "font/ttf"},
    {".txt", "text/plain"},
    {".vsd", "application/vnd.visio"},
    {".wav", "audio/wav"},
    {".weba", "audio/webm"},
    {".webm", "video/webm"},
    {".webp", "image/webp"},
    {".woff", "font/woff"},
    {".woff2", "font/woff2"},
    {".xhtml", "application/xhtml+xml"},
    {".xls", "application/vnd.ms-excel"},
    {".xlsx", "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"},
    {".xml", "application/xml"},
    {".xul", "application/vnd.mozilla.xul+xml"},
    {".zip", "application/zip"},
    {".3gp", "video/3gpp"},
    {".3g2", "video/3gpp2"},
    {".7z", "application/x-7z-compressed"},
    {NULL, "application/octet-stream"}
};

// Memory Stream for curl
typedef struct MemoryStream {
    char *memory;
    size_t size;
} MemoryStream;

#endif