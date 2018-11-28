#ifndef AIMBRAIN_H
#define AIMBRAIN_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>
#include <openssl/buffer.h>

typedef struct AimbrainContext {
  const char* api_key;
  const char* secret;
} AimbrainContext;

typedef enum AimbrainErrorCode {
  AIMBRAIN_NO_ERROR,
  AIMBRAIN_MEMORY_ERROR,
  AIMBRAIN_NETWORK_ERROR
} AimbrainErrorCode;

typedef struct AimbrainError {
  AimbrainErrorCode code;
  const char* msg;
} AimbrainError;

typedef struct SessionsInput {
  const char* userID;
  const char* device;
  const char* system;
  int screenHeight;
  int screenWidth;
} SessionsInput;

typedef struct SessionsOutput {
  const char* session;
  int face;
  int voice;
  int behaviour;
  AimbrainError error;
} SessionsOutput;

AimbrainContext* Aimbrain_Init(const char* apiKey, const char* secret);
void Aimbrain_Dispose(AimbrainContext* ctx);

SessionsOutput Aimbrain_Sessions(AimbrainContext* ctx, SessionsInput input);

#endif
