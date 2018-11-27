#include "aimbrain.h"

typedef struct DoRequestOutput {
  int statusCode;
  cJSON* body;
  AimbrainError error;
} DoRequestOutput;

struct MemoryStruct {
  char *memory;
  size_t size;
};

static char* ConcatenateStrings(const char *str1, const char *str2);
static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);
static DoRequestOutput DoRequest(AimbrainContext* ctx, char* endpoint, cJSON* data);

AimbrainContext* Aimbrain_Init(const char* apiKey, const char* secret) {
  AimbrainContext* ctx = malloc(sizeof(AimbrainContext));

  if(ctx == NULL) {
    return NULL;
  }

  ctx->apiKey = apiKey;
  ctx->secret = secret;
  return ctx;
}

void Aimbrain_Dispose(AimbrainContext* ctx) {
  free(ctx);
}

SessionsOutput Aimbrain_Sessions(AimbrainContext* ctx, SessionsInput input) {
  SessionsOutput so;

  cJSON* reqBody = cJSON_CreateObject();

  cJSON* userID = cJSON_CreateString(input.userID);
  cJSON_AddItemToObject(reqBody, "userID", userID);

  cJSON* device = cJSON_CreateString(input.device);
  cJSON_AddItemToObject(reqBody, "device", device);

  cJSON* sys = cJSON_CreateString(input.system);
  cJSON_AddItemToObject(reqBody, "system", sys);

  cJSON* screenHeight = cJSON_CreateNumber(input.screenHeight);
  cJSON_AddItemToObject(reqBody, "screenHeight", screenHeight);

  cJSON* screenWidth = cJSON_CreateNumber(input.screenWidth);
  cJSON_AddItemToObject(reqBody, "screenWidth", screenWidth);

  DoRequestOutput response = DoRequest(ctx, "/v1/sessions", reqBody);

  so.error = response.error;

  if(response.error.code != AIMBRAIN_NO_ERROR) {
    return so;
  }

  char* string = cJSON_Print(response.body);
  printf("%s\n", string);

  return so;
}

static size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;

  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(ptr == NULL) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }

  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;

  return realsize;
}

static char* ConcatenateStrings(const char *str1, const char *str2) {
    char *finalString = NULL;
    size_t n = 0;

    if (str1) n += strlen(str1);
    if (str2) n += strlen(str2);

    if ((str1 || str2) && (finalString = malloc( n + 1 )) != NULL)
    {
        *finalString = '\0';

        if (str1) strcpy(finalString, str1);
        if (str2) strcat(finalString, str2);
    }

    return finalString;
}

static void GenerateBase64(const unsigned char* msg, size_t msgsize, char* out, size_t outsize) {
    BIO *bio, *b64, *mem;
    b64 = BIO_new(BIO_f_base64());

    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    mem = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, mem);
    BIO_write(bio, msg, msgsize);
    BIO_flush(bio);
    BIO_gets(mem, out, outsize);
    BIO_free_all(bio);
}

static DoRequestOutput DoRequest(AimbrainContext* ctx, char* endpoint, cJSON* data) {
  CURL *curl = curl_easy_init();
  struct MemoryStruct chunk;

  AimbrainError err;
  err.msg = "";
  err.code = AIMBRAIN_NO_ERROR;

  DoRequestOutput ro;
  ro.statusCode = 0;
  ro.error = err;
  ro.body = NULL;

  chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  chunk.size = 0;    /* no data at this point */

  if(curl) {
    CURLcode res;
    struct curl_slist *headers = NULL;

    char* url = ConcatenateStrings("https://api.aimbrain.com", endpoint);
    char* apiKeyHeader = ConcatenateStrings("X-aimbrain-apikey: ", ctx->apiKey);
    char* reqData = cJSON_PrintUnformatted(data);
    char* message = "";

    //Prepare message for HMAC
    message = ConcatenateStrings("POST\n", endpoint);
    message = ConcatenateStrings(message, "\n");
    message = ConcatenateStrings(message, reqData);

    printf("%s\n", message);
    printf("%s\n", ctx->secret);

/*
    unsigned char hmac[64];

    GenerateHMACSHA256(ctx->secret, message, hmac);
    GenerateBase64(hmac, 64, hmacb64, 32);*/
    unsigned char* result;
    unsigned int len = 64;

    result = (unsigned char*)malloc(sizeof(char) * len);

    HMAC_CTX *hctx;
    hctx = HMAC_CTX_new();

    HMAC_Init(hctx, ctx->secret, strlen(ctx->secret), EVP_sha256());
    HMAC_Update(hctx, (unsigned char*)reqData, strlen(reqData));
    HMAC_Final(hctx, result, &len);
    HMAC_CTX_get_md(hctx);

    char hmacb64[100];
    GenerateBase64(result, len, hmacb64, 100);

    printf("%s\n", hmacb64);

    free(result);

    char* secretHeader = ConcatenateStrings("X-aimbrain-signature: ", (const char *)hmacb64);

    headers = curl_slist_append(headers, "Expect:");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, apiKeyHeader);
    headers = curl_slist_append(headers, secretHeader);

    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, reqData);

    //curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    ro.statusCode = curl_easy_perform(curl);

    if(ro.statusCode != CURLE_OK) {
      err.msg = curl_easy_strerror(ro.statusCode);
      err.code = AIMBRAIN_LIBRARY_ERROR;

      ro.error = err;
      ro.body = NULL;

      free(url);
      free(apiKeyHeader);
      free(reqData);
      free(chunk.memory);

      return ro;
    }

    ro.body = cJSON_Parse(chunk.memory);

    //Dispose of used resources
    curl_easy_cleanup(curl);

    free(url);
    free(apiKeyHeader);
    free(reqData);
  }

  free(chunk.memory);
  return ro;
}
