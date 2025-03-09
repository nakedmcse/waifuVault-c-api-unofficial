// Test program for buckets api
#include<stdio.h>
#include<string.h>
#include<strings.h>
#include<unistd.h>
#include "../waifuvault-c-api.h"
#include "../waifuvault-c-models.h"

int main(void) {
	FileResponse normalFile, albumFile;
	BucketResponse bucket;

	static char *bucketJson = "{"
  "\"token\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
  "\"files\": ["
    "{"
      "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
      "\"retentionPeriod\": 28805053563,"
      "\"album\": {"
        "\"token\": \"3496e358-955f-4301-aeed-2f7446c45e81\","
        "\"publicToken\": null,"
        "\"name\": \"test-album\","
        "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
        "\"dateCreated\": 1738521377000"
      "},"
      "\"token\": \"63a1a691-1ab3-44d5-9102-37e817c4af3a\","
      "\"views\": 3, \"id\": 1,"
      "\"url\": \"http://localhost:8081/f/1738521461344/ronin-monster-2.jpeg\","
      "\"options\": {"
        "\"hideFilename\": false,"
        "\"oneTimeDownload\": false,"
        "\"protected\": false"
      "}"
    "},"
    "{"
      "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
      "\"retentionPeriod\": 28805198862,"
      "\"album\": null,"
      "\"token\": \"029a56c1-e5dd-47d0-a648-ea4838e2d134\","
      "\"views\": 2, \"id\": 2,"
      "\"url\": \"http://localhost:8081/f/1738521461351/ronin-monster.jpeg\","
      "\"options\": {"
        "\"hideFilename\": false,"
        "\"oneTimeDownload\": false,"
        "\"protected\": false"
      "}"
    "},"
    "{"
      "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
      "\"retentionPeriod\": 28798791034,"
      "\"album\": {"
        "\"token\": \"3496e358-955f-4301-aeed-2f7446c45e81\","
        "\"publicToken\": null,"
        "\"name\": \"test-album\","
        "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
        "\"dateCreated\": 1738521377000"
      "},"
      "\"token\": \"c0fe567e-e3ee-4809-b381-88262688917c\","
      "\"views\": 5, \"id\": 3,"
      "\"url\": \"http://localhost:8081/f/1738521461478/ronin-moon.jpeg\","
      "\"options\": {"
        "\"hideFilename\": false,"
        "\"oneTimeDownload\": false,"
        "\"protected\": false"
      "}"
    "}"
  "],"
  "\"albums\": ["
    "{"
      "\"token\": \"3496e358-955f-4301-aeed-2f7446c45e81\","
      "\"publicToken\": null,"
      "\"name\": \"test-album\","
      "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
      "\"dateCreated\": 1738521377000"
    "}"
  "]"
"}";

	static char *albumJson = "{"
  "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
  "\"retentionPeriod\": \"334 days 12 hours 53 minutes\","
  "\"album\": {"
  	"\"token\": \"3496e358-955f-4301-aeed-2f7446c45e81\","
	  "\"publicToken\": null,"
	  "\"name\": \"test-album\","
	  "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
	  "\"dateCreated\": 1738521377000"
	"},"
	"\"token\": \"63a1a691-1ab3-44d5-9102-37e817c4af3a\","
	"\"id\": 1,"
	"\"views\": 3,"
	"\"url\": \"http://localhost:8081/f/1738521461344/ronin-monster-2.jpeg\","
	"\"options\": {"
		"\"hideFilename\": false,"
		"\"oneTimeDownload\": false,"
		"\"protected\": false"
	  "}"
	"}";

	static char *noAlbumJson = "{"
  "\"bucket\": \"d15aff38-e786-4f9c-9e1c-b77cfe905bec\","
  "\"retentionPeriod\": \"334 days 13 hours 4 minutes 41 seconds\","
  "\"album\": null,"
  "\"token\": \"029a56c1-e5dd-47d0-a648-ea4838e2d134\","
	"\"id\": 2,"
  "\"views\": 1,"
  "\"url\": \"http://localhost:8081/f/1738521461351/ronin-monster.jpeg\","
  "\"options\": {"
  	"\"hideFilename\": false,"
	  "\"oneTimeDownload\": false,"
	  "\"protected\": false"
	"}"
	"}";

	normalFile = deserializeResponse(noAlbumJson, true);
	printf("No album file album name: %s\n", normalFile.album.name);
	printf("No album file id: %d\n", normalFile.id);

	albumFile = deserializeResponse(albumJson, true);
	printf("Album file album name: %s\n", albumFile.album.name);
	printf("Album file id: %d\n", albumFile.id);

	bucket = deserializeBucketResponse(bucketJson);
	printf("Bucket Token: %s\n", bucket.token);
	printf("Album Name: %s\n", bucket.albums.items[0].name);
	printf("File Ids: %d, %d, %d\n", bucket.files.items[0].id, bucket.files.items[1].id, bucket.files.items[2].id);

	return 0;
}