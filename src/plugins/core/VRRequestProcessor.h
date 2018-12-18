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
#ifndef VSHL_CORE_INCLUDE_VR_REQUESTPROCESSOR_H_
#define VSHL_CORE_INCLUDE_VR_REQUESTPROCESSOR_H_

#include <memory>
#include <unordered_map>

#include "core/include/VRRequest.h"
#include "core/include/VRRequestProcessorDelegate.h"
#include "interfaces/afb/IAFBApi.h"
#include "interfaces/utilities/logging/ILogger.h"
#include "interfaces/voiceagents/IVoiceAgentsChangeObserver.h"

using namespace std;

namespace vshl {
namespace core {
/*
 * This class is the entry point for all the voice recognition request
 * creation and arbitration.
 */
class VRRequestProcessor {
public:
    // Create a VRRequestProcessor.
    static unique_ptr<VRRequestProcessor> create(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::core::VRRequestProcessorDelegate> delegate);

    // Triggers a voiceagent to start listening to user speech input.
    // Returns the request ID. If start fails, then empty request ID
    // is returned.
    string startListening();

    // Cancels all the active requests
    void cancel();

    // Returns the voiceagents observer that belongs to the core module.
    shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver> getVoiceAgentsChangeObserver() const;

    // Destructor
    ~VRRequestProcessor();

private:
    // Constructor
    VRRequestProcessor(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::core::VRRequestProcessorDelegate> delegate);

    // Voiceagents observer
    shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver> mVoiceAgentsChangeObserver;

    // Request Processor Delegate
    shared_ptr<vshl::core::VRRequestProcessorDelegate> mDelegate;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace core
}  // namespace vshl

#endif  // VSHL_CORE_INCLUDE_VR_REQUESTPROCESSOR_H_
