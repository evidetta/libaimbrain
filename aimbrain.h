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

typedef enum AimbrainErrorCode {
  AIMBRAIN_NO_ERROR,
  AIMBRAIN_LIBRARY_ERROR
} AimbrainErrorCode;

typedef struct AimbrainError {
  AimbrainErrorCode code;
  const char* msg;
} AimbrainError;

typedef struct AimbrainContext {
  const char* apiKey;
  const char* secret;
} AimbrainContext;

typedef struct SessionsInput {
  const char* userID;
  const char* device;
  const char* system;
  int screenHeight;
  int screenWidth;
} SessionsInput;

typedef struct SessionsOutput {
  const char* sessions;
  int face;
  int voice;
  int behaviour;
  AimbrainError error;
} SessionsOutput;

AimbrainContext* Aimbrain_Init(const char* apiKey, const char* secret);
void Aimbrain_Dispose(AimbrainContext* ctx);

SessionsOutput Aimbrain_Sessions(AimbrainContext* ctx, SessionsInput input);

#endif
