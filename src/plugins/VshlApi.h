/*
 * Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License").
 * You may not use this file except in compliance with the License.
 * A copy of the License is located at
 *
 *     http://aws.amazon.com/apache2.0/
 *
 * or in the "license" file accompanying this file. This file is distributed
 * on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing
 * permissions and limitations under the License.
 */
#ifndef VSHL_API_INCLUDE
#define VSHL_API_INCLUDE

#include "ctl-plugin.h"

#ifdef __cplusplus
extern "C" {
#endif

CTLP_ONLOAD(plugin, ret);
CTLP_INIT(plugin, ret);
int onAuthStateEvent(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int onConnectionStateEvent(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int onDialogStateEvent(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int loadVoiceAgentsConfig(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int startListening(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int cancelListening(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int enumerateVoiceAgents(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int subscribe(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int setDefaultVoiceAgent(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int guiMetadataSubscribe(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int guiMetadataPublish(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int phonecontrolSubscribe(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int phonecontrolPublish(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int navigationSubscribe(CtlSourceT* source, json_object* argsJ, json_object* queryJ);
int navigationPublish(CtlSourceT* source, json_object* argsJ, json_object* queryJ);

#ifdef __cplusplus
}
#endif

#endif  // VSHL_API_INCLUDE
