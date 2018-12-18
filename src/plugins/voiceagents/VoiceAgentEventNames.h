/*
 * Copyright 2018-2019 Amazon.com, Inc. or its affiliates. All Rights Reserved.
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
#ifndef VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTEVENTNAMES_H_
#define VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTEVENTNAMES_H_

#include <list>
#include <string>

using namespace std;

namespace vshl {
namespace voiceagents {

static string VSHL_EVENT_AUTH_STATE_EVENT = "voice_authstate_event";
static string VSHL_EVENT_CONNECTION_STATE_EVENT = "voice_connectionstate_event";
static string VSHL_EVENT_DIALOG_STATE_EVENT = "voice_dialogstate_event";

static list<string> VSHL_EVENTS = {
    VSHL_EVENT_AUTH_STATE_EVENT, VSHL_EVENT_CONNECTION_STATE_EVENT,
    VSHL_EVENT_DIALOG_STATE_EVENT,
};

} // namespace voiceagents
} // namespace vshl

#endif // VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTEVENTNAMES_H_
