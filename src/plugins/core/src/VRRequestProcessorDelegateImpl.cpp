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
#include "core/include/VRRequestProcessorDelegate.h"

static string TAG = "vshl::core::VRRequestProcessorDelegate";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace core {
shared_ptr<VRRequestProcessorDelegate> VRRequestProcessorDelegate::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) {
    auto delegate = std::shared_ptr<VRRequestProcessorDelegate>(new VRRequestProcessorDelegate(logger, afbApi));
    return delegate;
}

VRRequestProcessorDelegate::VRRequestProcessorDelegate(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) :
        mApi(afbApi),
        mLogger(logger) {
}

VRRequestProcessorDelegate::~VRRequestProcessorDelegate() {
    mVRRequests.clear();
}

string VRRequestProcessorDelegate::startRequestForVoiceAgent(
    shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
    if (!mApi) {
        mLogger->log(Level::ERROR, TAG, "Failed to startRequestForVoiceAgent: " + voiceAgent->getId() + ", No API.");
        return "";
    }

    // Generate a new request ID.
    string newReqId = vshl::utilities::uuid::generateUUID();

    // Create a new request and start listening.
    shared_ptr<VRRequest> newRequest = VRRequest::create(mLogger, mApi, newReqId, voiceAgent);

    // mLogger->log(Level::DEBUG, TAG, "Starting request with ID: " + newReqId);
    if (!newRequest->startListening()) {
        mLogger->log(Level::ERROR, TAG, "Failed to start listening.");
        return "";
    }

    // Insert only if its started successfully.
    mVRRequests.insert(make_pair(voiceAgent->getId(), newRequest));

    return newReqId;
}

void VRRequestProcessorDelegate::cancelAllRequests() {
    // Cancel Pending requests
    if (!mVRRequests.empty()) {
        auto vrRequestsIt = mVRRequests.begin();
        while (vrRequestsIt != mVRRequests.end()) {
            if (!vrRequestsIt->second->cancel()) {
                mLogger->log(Level::WARNING, TAG, "Failed to cancel request: " + vrRequestsIt->first);
            }
            vrRequestsIt++;
        }
        mVRRequests.clear();
    }
}

unordered_map<string, shared_ptr<VRRequest>> VRRequestProcessorDelegate::getAllRequests() {
    return mVRRequests;
}

void VRRequestProcessorDelegate::setDefaultVoiceAgent(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
    mDefaultVoiceAgent = voiceAgent;
}

shared_ptr<vshl::common::interfaces::IVoiceAgent> VRRequestProcessorDelegate::getDefaultVoiceAgent() const {
    return mDefaultVoiceAgent;
}

}  // namespace core
}  // namespace vshl
