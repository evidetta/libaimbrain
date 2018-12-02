#include "request.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>

typedef struct MemoryStruct {
  char *memory;
  size_t size;
} MemoryStruct;


static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  MemoryStruct *mem = (MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

static void SHA256HMAC(const char* key, const char* message, unsigned char* result, unsigned int* len) {
  HMAC_CTX *ctx;
  ctx = HMAC_CTX_new();

  HMAC_Init_ex(ctx, key, strlen(key), EVP_sha256(), NULL);
  HMAC_Update(ctx, (unsigned char*)message, strlen(message));
  HMAC_Final(ctx, result, len);
  HMAC_CTX_free(ctx);
}

static void Base64Encode(const unsigned char* buffer, size_t length, char** b64text) {
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	*b64text=(*bufferPtr).data;
}

Response MakeRequest(AimbrainContext* ctx, Request request) {
  AimbrainError err = {AIMBRAIN_NO_ERROR, ""};
  Response response = {0, NULL, err};

  //Concatenate URL from host and endpoint.
  char* url = (char *)malloc(sizeof(char) * (strlen(request.host) + strlen(request.endpoint) + 1));
  if(url == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for url failed."};
    response.error = err;
    goto cleanup_url;
  }

  strncpy(url, request.host, strlen(request.host));
  strncat(url, request.endpoint, strlen(request.endpoint));

  //Concatenate API Key Header.
  char* api_key_header_key = "X-aimbrain-apikey: ";
  char* api_key_header = (char *)malloc(sizeof(char) * (strlen(api_key_header_key) + strlen(ctx->api_key) + 1));
  if(api_key_header == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for api_key_header failed."};
    response.error = err;
    goto cleanup_api_key_header;
  }

  strncpy(api_key_header, api_key_header_key, strlen(api_key_header_key));
  strncat(api_key_header, ctx->api_key, strlen(ctx->api_key));

  //Generate SHA256-HMAC using secret and payload.
  char* request_method = "POST";
  char* request_data = cJSON_PrintUnformatted(request.body);
  char* payload = (char *)malloc(sizeof(char) * (strlen(request_method) + strlen(request.endpoint) + strlen(request_data) + 3));
  if(payload == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for payload failed."};
    response.error = err;
    goto cleanup_payload;
  }

  char* carriage_return = "\n";

  strcpy(payload, request_method); //TODO: Investigate this hack.
  strncat(payload, carriage_return, strlen(carriage_return));
  strncat(payload, request.endpoint, strlen(request.endpoint));
  strncat(payload, carriage_return, strlen(carriage_return));
  strncat(payload, request_data, strlen(request_data));

  MemoryStruct raw_hash;
  raw_hash.size = SHA256_BLOCK_SIZE; //Output buffer size for SHA256 output.
  raw_hash.memory = (char *)malloc(sizeof(char) * raw_hash.size);
  if(raw_hash.memory == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for raw_hash.memory failed."};
    response.error = err;
    goto cleanup_raw_hash_memory;
  }

  SHA256HMAC(ctx->secret, payload, (unsigned char *)raw_hash.memory, (unsigned int *)&raw_hash.size);

  //Base64-encode the hash.
  char* b64_hash;
  Base64Encode((const unsigned char *)raw_hash.memory, raw_hash.size, &b64_hash);

  //Concatenate Signature Header.
  char* signature_header_key = "X-aimbrain-signature: ";
  char* signature_header = (char *)malloc(sizeof(char) * (strlen(signature_header_key) + strlen(b64_hash) + 1));
  if(signature_header == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for signature_header failed."};
    response.error = err;
    goto cleanup_signature_header;
  }

  strncpy(signature_header, signature_header_key, strlen(signature_header_key));
  strncat(signature_header, b64_hash, strlen(b64_hash));

  //Set up CURL
  CURL *curl = curl_easy_init();
  if(curl == NULL) {
    goto cleanup_curl;
  }

  //Set up CURL headers
  struct curl_slist *headers = NULL;

  headers = curl_slist_append(headers, "Expect:");
  if(headers == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for Expect header failed."};
    response.error = err;
    goto cleanup_headers;
  }

  headers = curl_slist_append(headers, "Content-Type: application/json");
  if(headers == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for Content-Type header failed."};
    response.error = err;
    goto cleanup_headers;
  }

  headers = curl_slist_append(headers, api_key_header);
  if(headers == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for X-aimbrain-apikey header failed."};
    response.error = err;
    goto cleanup_headers;
  }

  headers = curl_slist_append(headers, signature_header);
  if(headers == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for X-aimbrain-signature header failed."};
    response.error = err;
    goto cleanup_headers;
  }

  //Set CURL Options
  char curl_error_buffer[CURL_ERROR_SIZE];

  MemoryStruct response_buffer;

  response_buffer.size = 0;
  response_buffer.memory = (char *)malloc(sizeof(char));
  if(response_buffer.memory == NULL) {
    AimbrainError err = {AIMBRAIN_MEMORY_ERROR, "Memory allocation for response_buffer.memory failed."};
    response.error = err;
    goto cleanup_response_buffer_memory;
  }

  curl_easy_setopt(curl, CURLOPT_URL, url);
  curl_easy_setopt(curl, CURLOPT_POST, 1L);
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_data);
  curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, curl_error_buffer);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&response_buffer);
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

  CURLcode res;
  res = curl_easy_perform(curl);
  if(res != CURLE_OK) {
    AimbrainError err = {AIMBRAIN_NETWORK_ERROR, curl_easy_strerror(res)};
    response.error = err;

    size_t len = strlen(curl_error_buffer);
    fprintf(stderr, "\nlibcurl: (%d) ", res);
    if(len) {
      fprintf(stderr, "%s%s", curl_error_buffer, ((curl_error_buffer[len - 1] != '\n') ? "\n" : ""));
    }
    else {
      fprintf(stderr, "%s\n", curl_easy_strerror(res));
    }
  }

  curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &response.status_code);
  response.body = cJSON_Parse(response_buffer.memory);

//Cleanup before exit
cleanup_response_buffer_memory:
  free(response_buffer.memory);
cleanup_headers:
  curl_slist_free_all(headers);
cleanup_curl:
  curl_easy_cleanup(curl);
cleanup_signature_header:
  free(signature_header);
cleanup_raw_hash_memory:
  free(raw_hash.memory);
cleanup_payload:
  free(payload);
cleanup_api_key_header:
  free(api_key_header);
cleanup_url:
  free(url);

  return response;
}
