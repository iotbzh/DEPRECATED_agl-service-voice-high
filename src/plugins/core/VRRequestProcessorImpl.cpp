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
#include "core/VRRequestProcessor.h"

#include "core/include/VRAgentsObserver.h"

static string TAG = "vshl::core::VRRequestProcessor";

using Level = vshl::utilities::logging::Logger::Level;

namespace vshl {
namespace core {
// Create a VRRequestProcessor.
unique_ptr<VRRequestProcessor> VRRequestProcessor::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::core::VRRequestProcessorDelegate> delegate) {
    auto processor = std::unique_ptr<VRRequestProcessor>(new VRRequestProcessor(logger, delegate));
    return processor;
}

VRRequestProcessor::VRRequestProcessor(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::core::VRRequestProcessorDelegate> delegate) :
        mLogger(logger),
        mDelegate(delegate) {
    mVoiceAgentsChangeObserver = VRAgentsObserver::create(mDelegate);
}

VRRequestProcessor::~VRRequestProcessor() {
    mDelegate->cancelAllRequests();
}

string VRRequestProcessor::startListening() {
    // Currently start is simply going to send the request to
    // the default voice agent as the wake word detection is not
    // enabled.
    shared_ptr<vshl::common::interfaces::IVoiceAgent> defaultVA = mDelegate->getDefaultVoiceAgent();
    if (!defaultVA) {
        mLogger->log(Level::ERROR, TAG, "Failed to start. No default voiceagent found.");
        return "";
    }

    // If the requests container is not empty, then clear the
    // existing requests in flight and create a new request.
    mDelegate->cancelAllRequests();
    return mDelegate->startRequestForVoiceAgent(defaultVA);
}

void VRRequestProcessor::cancel() {
    // Cancel all pending requests
    mDelegate->cancelAllRequests();
}

shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver> VRRequestProcessor::getVoiceAgentsChangeObserver()
    const {
    return mVoiceAgentsChangeObserver;
}

}  // namespace core
}  // namespace vshl
