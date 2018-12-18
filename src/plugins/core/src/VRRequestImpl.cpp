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
#include "core/include/VRRequest.h"

#define FREEIF(x) \
    if (!x) {     \
        free(x);  \
    }
#define BREAKIF(x)      \
    if (x) {            \
        result = false; \
        break;          \
    }

static string TAG = "vshl::core::VRRequest";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace core {

string VRRequest::VA_VERB_STARTLISTENING = "startListening";
string VRRequest::VA_VERB_CANCEL = "cancel";

unique_ptr<VRRequest> VRRequest::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
    const string requestId,
    shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
    if (logger == nullptr) {
        return nullptr;
    }

    if (afbApi == nullptr) {
        logger->log(Level::ERROR, TAG, "Invalid AFB API");
        return nullptr;
    }

    auto request = std::unique_ptr<VRRequest>(new VRRequest(logger, afbApi, requestId, voiceAgent));
    return request;
}

VRRequest::VRRequest(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
    string requestId,
    shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) :
        mApi(afbApi),
        mRequestId(requestId),
        mVoiceAgent(voiceAgent),
        mLogger(logger) {
}

VRRequest::~VRRequest() {
}

bool VRRequest::startListening() {
    json_object* object = NULL;
    std::string error, info;
    bool result = true;
    int rc = mApi->callSync(mVoiceAgent->getApi(), VA_VERB_STARTLISTENING, NULL, &object, error, info);

    FREEIF(object);

    return true;
}

bool VRRequest::cancel() {
    json_object* object = NULL;
    std::string error, info;
    bool result = true;
    int rc = mApi->callSync(mVoiceAgent->getApi(), VA_VERB_CANCEL, NULL, &object, error, info);

    FREEIF(object);

    return true;
}

}  // namespace core
}  // namespace vshl
