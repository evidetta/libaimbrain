#include "request.h"
#include "../error/error.h"

#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <openssl/bio.h>
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

static char* Base64Encode(const unsigned char* buffer, size_t length) {
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, buffer, length);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);

  char* b64_str = (char *)malloc(bufferPtr->length);
  memset(b64_str, 0, bufferPtr->length);
  memcpy(b64_str, bufferPtr->data, bufferPtr->length - 1);

  BIO_free_all(bio);
  return b64_str;
}

Response MakeRequest(AimbrainContext* ctx, Request request) {
  AimbrainError err = {AIMBRAIN_NO_ERROR, ""};
  Response response = {0, NULL, err};

  //Concatenate URL from host and endpoint.
  MemoryStruct url;
  url.size = sizeof(char) * (strlen(request.host) + strlen(request.endpoint) + 1);
  url.memory = (char *)malloc(url.size);
  if(url.memory == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for url failed.");
    response.error = err;
    goto cleanup_url_memory;
  }

  memset(url.memory, 0, url.size);
  strncpy(url.memory, request.host, strlen(request.host));
  strncat(url.memory, request.endpoint, strlen(request.endpoint));

  //Concatenate API Key Header.
  char* api_key_header_key = "X-aimbrain-apikey: ";

  MemoryStruct api_key_header;
  api_key_header.size = sizeof(char) * (strlen(api_key_header_key) + strlen(ctx->api_key) + 1);
  api_key_header.memory = (char *)malloc(api_key_header.size);
  if(api_key_header.memory == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for api_key_header failed.");
    response.error = err;
    goto cleanup_api_key_header_memory;
  }

  memset(api_key_header.memory, 0, api_key_header.size);
  strncpy(api_key_header.memory, api_key_header_key, strlen(api_key_header_key));
  strncat(api_key_header.memory, ctx->api_key, strlen(ctx->api_key));

  //Generate SHA256-HMAC using secret and payload.
  char* request_method = "POST";

  char* request_data = cJSON_PrintUnformatted(request.body);
  if(request_data == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for payload failed.");
    response.error = err;
    goto cleanup_request_data;
  }

  MemoryStruct payload;
  payload.size = sizeof(char) * (strlen(request_method) + strlen(request.endpoint) + strlen(request_data) + 3);
  payload.memory = (char *)malloc(payload.size);
  if(payload.memory == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for payload failed.");
    response.error = err;
    goto cleanup_payload_memory;
  }

  char* carriage_return = ENDL;

  memset(payload.memory, 0, payload.size);
  strncpy(payload.memory, request_method, strlen(request_method));
  strncat(payload.memory, carriage_return, strlen(carriage_return));
  strncat(payload.memory, request.endpoint, strlen(request.endpoint));
  strncat(payload.memory, carriage_return, strlen(carriage_return));
  strncat(payload.memory, request_data, strlen(request_data));

  MemoryStruct raw_hash;
  raw_hash.size = SHA256_BLOCK_SIZE; //Output buffer size for SHA256 output.
  raw_hash.memory = (char *)malloc(sizeof(char) * raw_hash.size);
  if(raw_hash.memory == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for raw_hash.memory failed.");
    response.error = err;
    goto cleanup_raw_hash_memory;
  }

  SHA256HMAC(ctx->secret, payload.memory, (unsigned char *)raw_hash.memory, (unsigned int *)&raw_hash.size);

  //Base64-encode the hash.
  char* b64_hash = Base64Encode((const unsigned char *)raw_hash.memory, raw_hash.size);
  if(b64_hash == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for b64_hash failed.");
    response.error = err;
    goto cleanup_b64_hash;
  }

  //Concatenate Signature Header.
  char* signature_header_key = "X-aimbrain-signature: ";
  MemoryStruct signature_header;
  signature_header.size = sizeof(char) * (strlen(signature_header_key) + strlen(b64_hash) + 1);
  signature_header.memory = (char *)malloc(signature_header.size);
  if(signature_header.memory == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for signature_header failed.");
    response.error = err;
    goto cleanup_signature_header_memory;
  }

  memset(signature_header.memory, 0, signature_header.size);
  strncpy(signature_header.memory, signature_header_key, strlen(signature_header_key));
  strncat(signature_header.memory, b64_hash, strlen(b64_hash));

  //Set up CURL
  CURL *curl = curl_easy_init();
  if(curl == NULL) {
    goto cleanup_curl;
  }

  //Set up CURL headers
  struct curl_slist *headers = NULL;

  headers = curl_slist_append(headers, "Expect:");
  if(headers == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for Expect header failed.");
    response.error = err;
    goto cleanup_headers;
  }

  headers = curl_slist_append(headers, "Content-Type: application/json");
  if(headers == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for Content-Type header failed.");
    response.error = err;
    goto cleanup_headers;
  }

  headers = curl_slist_append(headers, api_key_header.memory);
  if(headers == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for X-aimbrain-apikey header failed.");
    response.error = err;
    goto cleanup_headers;
  }

  headers = curl_slist_append(headers, signature_header.memory);
  if(headers == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for X-aimbrain-signature header failed.");
    response.error = err;
    goto cleanup_headers;
  }

  //Set CURL Options
  char curl_error_buffer[CURL_ERROR_SIZE];
  memset(curl_error_buffer, 0, CURL_ERROR_SIZE);

  MemoryStruct response_buffer;

  response_buffer.size = 0;
  response_buffer.memory = (char *)malloc(sizeof(char));
  if(response_buffer.memory == NULL) {
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for response_buffer.memory failed.");
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
    AimbrainError err = GetNewAimbrainError(ctx, AIMBRAIN_NETWORK_ERROR, (char *)curl_easy_strerror(res));
    response.error = err;

    size_t len = strlen(curl_error_buffer);
    if(len) {
      SetErrorMessage(ctx, curl_error_buffer);
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
cleanup_signature_header_memory:
  free(signature_header.memory);
cleanup_b64_hash:
  free(b64_hash);
cleanup_raw_hash_memory:
  free(raw_hash.memory);
cleanup_payload_memory:
  free(payload.memory);
cleanup_request_data:
  free(request_data);
cleanup_api_key_header_memory:
  free(api_key_header.memory);
cleanup_url_memory:
  free(url.memory);

  return response;
}
