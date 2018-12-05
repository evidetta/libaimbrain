#ifndef AIMBRAIN_VOICE_H
#define AIMBRAIN_VOICE_H

#include "aimbrain.h"

typedef struct VoiceData {
  char* data;
  long size;
} VoiceData;

typedef struct VoiceTokenInput {
  char const * session;
  AimbrainTokenType token_type;
} VoiceTokenInput;

typedef struct VoiceTokenOutput {
  char * token;
} VoiceTokenOutput;

typedef struct VoiceEnrollInput {
  char const * session;
  VoiceData* voices;
} VoiceEnrollInput;

typedef struct VoiceEnrollOutput {
  int voice_samples;
} VoiceEnrollOutput;

typedef struct VoiceAuthInput {
  char const * session;
  VoiceData* voices;
} VoiceAuthInput;

typedef struct VoiceAuthOutput {
  float score;
  float liveliness;
} VoiceAuthOutput;

AimbrainError Aimbrain_VoiceToken(AimbrainContext* ctx, AimbrainVoiceTokenInput input, AimbrainVoiceTokenOutput** output);
AimbrainError Aimbrain_VoiceEnroll(AimbrainContext* ctx, AimbrainVoiceEnrollInput input, AimbrainVoiceEnrollOutput** output);
AimbrainError Aimbrain_VoiceAuth(AimbrainContext* ctx, AimbrainVoiceAuthInput input, AimbrainVoiceAuthOutput** output);

void Aimbrain_DisposeAimbrainVoiceTokenOutput(AimbrainVoiceTokenOutput* output);
void Aimbrain_DisposeAimbrainVoiceEnrollOutput(AimbrainVoiceEnrollOutput* output);
void Aimbrain_DisposeAimbrainVoiceAuthOutput(AimbrainVoiceAuthOutput* output);

#endif
