#ifndef AIMBRAIN_H
#define AIMBRAIN_H

//
#define AIMBRAIN_HOST "https://api.aimbrain.com"

//General Structs
typedef struct AimbrainContext {
  char const * api_key;
  char const * secret;
  char * error;
} AimbrainContext;

typedef enum AimbrainErrorCode {
  AIMBRAIN_NO_ERROR,
  AIMBRAIN_MEMORY_ERROR,
  AIMBRAIN_NETWORK_ERROR,
  AIMBRAIN_SERVICE_ERROR
} AimbrainErrorCode;

typedef struct AimbrainError {
  AimbrainErrorCode code;
  char const * msg;
} AimbrainError;

typedef enum AimbrainTokenType {
  AIMBRAIN_ENROLL_1,
  AIMBRAIN_ENROLL_2,
  AIMBRAIN_ENROLL_3,
  AIMBRAIN_ENROLL_4,
  AIMBRAIN_ENROLL_5,
  AIMBRAIN_AUTH
} AimbrainTokenType;

//Sessions
typedef struct AimbrainSessionsInput {
  char const * user_id;
  char const * device;
  char const * system;
  int screen_height;
  int screen_width;
} AimbrainSessionsInput;

typedef struct AimbrainSessionsOutput {
  char* session;
  int face;
  int voice;
  int behaviour;
} AimbrainSessionsOutput;

AimbrainContext* Aimbrain_Init(char const * apiKey, char const * secret);
void Aimbrain_Dispose(AimbrainContext* ctx);

AimbrainError Aimbrain_Sessions(AimbrainContext* ctx, AimbrainSessionsInput input, AimbrainSessionsOutput** output);
void Aimbrain_DisposeAimbrainSessionsOutput(AimbrainSessionsOutput* output);

#endif
