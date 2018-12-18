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
#ifndef VSHL_CORE_INCLUDE_VR_REQUEST_H_
#define VSHL_CORE_INCLUDE_VR_REQUEST_H_

#include <memory>

#include "interfaces/afb/IAFBApi.h"
#include "interfaces/utilities/logging/ILogger.h"
#include "interfaces/voiceagents/IVoiceAgent.h"

using namespace std;

namespace vshl {
namespace core {
/*
 * This class implements the notion of a Voice Recognition Request.
 * Each VR Request is currently mapped to one voice agent.
 */
class VRRequest {
public:
    // API Verbs
    static std::string VA_VERB_STARTLISTENING;
    static std::string VA_VERB_CANCEL;

    // Create a VRRequest.
    static unique_ptr<VRRequest> create(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
        string requestId,
        shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent);

    // Destructor
    ~VRRequest();

    // Invokes the underlying voiceagent's startlistening API.
    // Returns true if started successfully. False otherwise.
    bool startListening();

    // Cancels the voice recognition in the unlerlying voiceagent.
    // Returns true if canceled successfully. False otherwise.
    bool cancel();

private:
    // Constructor
    VRRequest(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
        const string requestId,
        shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent);

    // Binding API reference.
    shared_ptr<vshl::common::interfaces::IAFBApi> mApi;

    // Voice agent associated with this request
    shared_ptr<vshl::common::interfaces::IVoiceAgent> mVoiceAgent;

    // Request ID
    string mRequestId;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace core
}  // namespace vshl

#endif  // VSHL_CORE_INCLUDE_VR_REQUEST_H_
