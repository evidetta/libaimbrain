#include "aimbrain.h"
#include "../request/request.h"
#include "../error/error.h"

#include <cjson/cJSON.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


AimbrainError Aimbrain_Sessions(AimbrainContext* ctx, AimbrainSessionsInput input, AimbrainSessionsOutput** output) {
  AimbrainError err = {AIMBRAIN_NO_ERROR, ""};

  cJSON* obj = cJSON_CreateObject();
  if(obj == NULL) {
    err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for obj failed.");
    goto cleanup_obj;
  }

  cJSON* user_id = cJSON_CreateString(input.user_id);
  if(user_id == NULL) {
    err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for user_id failed.");
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "userID", user_id);

  cJSON* device = cJSON_CreateString(input.device);
  if(device == NULL) {
    err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for device failed.");
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "device", device);

  cJSON* system = cJSON_CreateString(input.system);
  if(system == NULL) {
    err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for system failed.");
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "system", system);

  cJSON* screen_height = cJSON_CreateNumber(input.screen_height);
  if(screen_height == NULL) {
    err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for screen_height failed.");
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "screenHeight", screen_height);

  cJSON* screen_width = cJSON_CreateNumber(input.screen_width);
  if(screen_width == NULL) {
    err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for screen_width failed.");
    goto cleanup_obj;
  }

  cJSON_AddItemToObject(obj, "screenWidth", screen_width);

  Request request = {AIMBRAIN_HOST, "/v1/sessions", obj};
  Response response = MakeRequest(ctx, request);

  if(response.error.code != AIMBRAIN_NO_ERROR) {
    err = response.error;
    goto internal_error;
  }

  cJSON* error = cJSON_GetObjectItemCaseSensitive(response.body, "error");
  if (cJSON_IsString(error) && (error->valuestring != NULL)) {
    switch(response.status_code) {
      case 400:
        err = GetNewAimbrainError(ctx, AIMBRAIN_SERVICE_ERROR, "Returned HTTP 400: Bad Request");
        break;
      case 401:
        err = GetNewAimbrainError(ctx, AIMBRAIN_SERVICE_ERROR, "Returned HTTP 401: Unauthorized");
        break;
      case 403:
        err = GetNewAimbrainError(ctx, AIMBRAIN_SERVICE_ERROR, "Returned HTTP 403: Forbidden");
        break;
      case 500:
        err = GetNewAimbrainError(ctx, AIMBRAIN_SERVICE_ERROR, "Returned HTTP 500: Internal Server Error");
        break;
      default:
        err = GetNewAimbrainError(ctx, AIMBRAIN_SERVICE_ERROR, "Returned unexpected code");
        break;
    }

    SetErrorMessage(ctx, error->valuestring);
    goto service_error;
  }

  //Allocate new memory off the heap for AimbrainSessionsOutput
  *output = (AimbrainSessionsOutput *)malloc(sizeof(AimbrainSessionsOutput));
  if(*output == NULL) {
    err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for output failed.");
    goto cleanup_output;
  }

  cJSON* session = cJSON_GetObjectItemCaseSensitive(response.body, "session");
  if(cJSON_IsString(session) && (session->valuestring != NULL)) {
    (*output)->session = (char *)malloc(strlen(session->valuestring));
    if((*output)->session == NULL) {
      free(*output);
      err = GetNewAimbrainError(ctx, AIMBRAIN_MEMORY_ERROR, "Memory allocation for output.session failed.");
      goto cleanup_output_session;
    }
    strncpy(((*output)->session), session->valuestring, strlen(session->valuestring));
  }

  cJSON* face = cJSON_GetObjectItemCaseSensitive(response.body, "face");
  if(cJSON_IsNumber(face)) {
    (*output)->face = face->valueint;
  }

  cJSON* voice = cJSON_GetObjectItemCaseSensitive(response.body, "voice");
  if(cJSON_IsNumber(voice)) {
    (*output)->voice = voice->valueint;
  }

  cJSON* behaviour = cJSON_GetObjectItemCaseSensitive(response.body, "behaviour");
  if(cJSON_IsNumber(behaviour)) {
    (*output)->behaviour = behaviour->valueint;
  }

cleanup_output_session:
cleanup_output:
service_error:
  cJSON_Delete(response.body);
internal_error:
cleanup_obj:
  cJSON_Delete(obj);

  return err;
}

void Aimbrain_DisposeAimbrainSessionsOutput(AimbrainSessionsOutput* output) {
  if(output != NULL) {
    printf("HERE");
    free(output->session);
    free(output);
    printf("Disposed OK\n");
  }
}
