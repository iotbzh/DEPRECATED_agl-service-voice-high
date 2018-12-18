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
#ifndef VSHL_CORE_INCLUDE_VR_REQUESTPROCESSORDELEGATE_H_
#define VSHL_CORE_INCLUDE_VR_REQUESTPROCESSORDELEGATE_H_

#include <memory>
#include <unordered_map>

#include "core/include/VRRequest.h"
#include "interfaces/afb/IAFBApi.h"
#include "interfaces/utilities/logging/ILogger.h"
#include "interfaces/voiceagents/IVoiceAgent.h"
#include "utilities/uuid/UUIDGeneration.h"

using namespace std;

namespace vshl {
namespace core {
/*
 * This is a delegate for VRRequestProcessor actions.
 * The lifetime and dependencies of this object is managed
 * by VRRequestProcessor. It plays the role of a Delegate in
 * Delegate pattern and a one shop stop for most of the core
 * module's state.
 */
class VRRequestProcessorDelegate {
public:
    // create method
    static shared_ptr<VRRequestProcessorDelegate> create(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    // Destructor
    ~VRRequestProcessorDelegate();

    // Set default voiceagent
    void setDefaultVoiceAgent(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent);

    // Get the default voiceagent
    shared_ptr<vshl::common::interfaces::IVoiceAgent> getDefaultVoiceAgent() const;

    // Add new request to the list and start processing it.
    // New request is created and startListening on the
    // voiceagent is called.
    string startRequestForVoiceAgent(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent);

    // Cancel all requests
    void cancelAllRequests();

    // Get All outstanding requests
    // Used only by Test
    unordered_map<string, shared_ptr<VRRequest>> getAllRequests();

private:
    // Constructor
    VRRequestProcessorDelegate(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    // Binding API reference
    shared_ptr<vshl::common::interfaces::IAFBApi> mApi;

    // Default voiceagent
    shared_ptr<vshl::common::interfaces::IVoiceAgent> mDefaultVoiceAgent;

    // A map of voiceagent IDs and their respective VR Request objects.
    unordered_map<string, shared_ptr<VRRequest>> mVRRequests;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace core
}  // namespace vshl

#endif  // VSHL_CORE_INCLUDE_VR_REQUESTPROCESSORDELEGATE_H_
