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
#include <sstream>

#include "voiceagents/include/VoiceAgent.h"

static string TAG = "vshl::voiceagents::VoiceAgent";

/**
 * Specifies the severity level of a log message
 */
using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace voiceagents {
// Creates @c VoiceAgent instance
shared_ptr<VoiceAgent> VoiceAgent::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    const string& id,
    const string& name,
    const string& description,
    const string& api,
    const string& vendor,
    const string& activeWakeword,
    const bool isActive,
    const shared_ptr<unordered_set<string>> wakewords) {
    if (wakewords == nullptr) {
        logger->log(Level::ERROR, TAG, "Wakeword list null");
        return nullptr;
    }

    auto voiceAgent = std::unique_ptr<VoiceAgent>(
        new VoiceAgent(logger, id, name, description, api, vendor, activeWakeword, isActive, wakewords));
    if (!voiceAgent->setActiveWakeWord(activeWakeword)) {
        return nullptr;
    }

    return voiceAgent;
}

VoiceAgent::VoiceAgent(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    const string& id,
    const string& name,
    const string& description,
    const string& api,
    const string& vendor,
    const string& activeWakeword,
    const bool isActive,
    const shared_ptr<unordered_set<string>> wakewords) :
        mLogger(logger),
        mId(id),
        mName(name),
        mDescription(description),
        mApi(api),
        mVendor(vendor),
        mActiveWakeword(activeWakeword),
        mIsActive(isActive),
        mWakewords(wakewords) {
}

// Destructor
VoiceAgent::~VoiceAgent() {
}

// Set the active wakeword for this voiceagent
bool VoiceAgent::setActiveWakeWord(const string& wakeword) {
    if (mWakewords->find(wakeword) != mWakewords->end()) {
        mActiveWakeword = wakeword;
        return true;
    }

    mLogger->log(Level::ERROR, TAG, "Wakeword: " + wakeword + " doesn't exist in wakeword list");
    return false;
}

// Sets the activation state of this voiceagent
void VoiceAgent::setIsActive(bool active) {
    mIsActive = active;
}

string VoiceAgent::getId() const {
    return mId;
}

string VoiceAgent::getName() const {
    return mName;
}

string VoiceAgent::getDescription() const {
    return mDescription;
}

string VoiceAgent::getApi() const {
    return mApi;
}

string VoiceAgent::getVendor() const {
    return mVendor;
}

shared_ptr<unordered_set<string>> VoiceAgent::getWakeWords() const {
    return mWakewords;
}

bool VoiceAgent::isActive() const {
    return mIsActive;
}

string VoiceAgent::getActiveWakeword() const {
    return mActiveWakeword;
}
}  // namespace voiceagents
}  // namespace vshl
