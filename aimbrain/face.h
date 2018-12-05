#ifndef AIMBRAIN_FACE_H
#define AIMBRAIN_FACE_H

#include "aimbrain.h"

typedef struct FaceData {
  char* data;
  long size;
} FaceData;

typedef struct FaceTokenInput {
  char const * session;
  AimbrainTokenType token_type;
} FaceTokenInput;

typedef struct FaceTokenOutput {
  char * token;
} FaceTokenOutput;

typedef struct FaceEnrollInput {
  char const * session;
  FaceData* faces;
} FaceEnrollInput;

typedef struct FaceEnrollOutput {
  int images_count;
} FaceEnrollOutput;

typedef struct FaceAuthInput {
  char const * session;
  FaceData* faces;
} FaceAuthInput;

typedef struct FaceAuthOutput {
  float score;
  float liveliness;
} FaceAuthOutput;

typedef struct FaceCompareInput {
  FaceData* faces_1;
  FaceData* faces_2;
} FaceCompareInput;

typedef struct FaceCompareOutput {
  float score;
  float liveliness_1;
  float liveliness_2;
}

AimbrainError Aimbrain_FaceToken(AimbrainContext* ctx, AimbrainFaceTokenInput input, AimbrainFaceTokenOutput** output);
AimbrainError Aimbrain_FaceEnroll(AimbrainContext* ctx, AimbrainFaceEnrollInput input, AimbrainFaceEnrollOutput** output);
AimbrainError Aimbrain_FaceAuth(AimbrainContext* ctx, AimbrainFaceAuthInput input, AimbrainFaceAuthOutput** output);
AimbrainError Aimbrain_FaceCompare(AimbrainContext* ctx, AimbrainFaceCompareInput input, AimbrainFaceCompareOutput** output);

void Aimbrain_DisposeAimbrainFaceTokenOutput(AimbrainFaceTokenOutput* output);
void Aimbrain_DisposeAimbrainFaceEnrollOutput(AimbrainFaceEnrollOutput* output);
void Aimbrain_DisposeAimbrainFaceAuthOutput(AimbrainFaceAuthOutput* output);
void Aimbrain_DisposeAimbrainFaceCompareOutput(AimbrainFaceCompareOutput* output);

#endif
