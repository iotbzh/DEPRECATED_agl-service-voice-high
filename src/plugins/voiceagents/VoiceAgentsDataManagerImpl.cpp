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
#include "voiceagents/VoiceAgentsDataManager.h"

#include "voiceagents/include/VoiceAgentEventsHandler.h"

static string TAG = "vshl::voiceagents::VoiceAgentsDataManager";
static string VA_VERB_START_SUBSCRIPTION_PROCESS = "startSubscriptionProcess";

/**
 * Specifies the severity level of a log message
 */
using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace voiceagents {

std::unique_ptr<VoiceAgentsDataManager> VoiceAgentsDataManager::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) {
    return std::unique_ptr<VoiceAgentsDataManager>(new VoiceAgentsDataManager(logger, afbApi));
}

// Constructor
VoiceAgentsDataManager::VoiceAgentsDataManager(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) :
        mLogger(logger),
        mAfbApi(afbApi) {
    mVoiceAgentEventsHandler = VoiceAgentEventsHandler::create(mLogger, mAfbApi);
    mAlreadyPeformedSubscriptions = false;
}

// Destructor
VoiceAgentsDataManager::~VoiceAgentsDataManager() {
    // Clear the observers
    mVoiceAgentChangeObservers.clear();
    // Clear the voiceagents
    mVoiceAgents.clear();
}

uint32_t VoiceAgentsDataManager::activateVoiceAgents(const unordered_set<string>& activeVoiceAgentIds) {
    if (activeVoiceAgentIds.empty() || mVoiceAgents.empty()) {
        mLogger->log(Level::ERROR, TAG, "Failed to activate voiceagents");
        return 0;
    }

    uint32_t agentsActivated = 0;
    for (auto voiceAgentId : activeVoiceAgentIds) {
        auto voiceAgentIt = mVoiceAgents.find(voiceAgentId);
        if (voiceAgentIt != mVoiceAgents.end()) {
            // activate the voiceagent
            ++agentsActivated;
            if (!voiceAgentIt->second->isActive()) {
                voiceAgentIt->second->setIsActive(true);
                // Notify observers
                for (auto observer : mVoiceAgentChangeObservers) {
                    observer->OnVoiceAgentActivated(voiceAgentIt->second);
                }
            }
        }
    }
    return agentsActivated;
}

uint32_t VoiceAgentsDataManager::deactivateVoiceAgents(const unordered_set<string>& inactiveVoiceAgentIds) {
    if (inactiveVoiceAgentIds.empty() || mVoiceAgents.empty()) {
        mLogger->log(Level::ERROR, TAG, "Failed to deactivate voiceagents");
        return 0;
    }

    uint32_t agentsDeactivated = 0;
    for (auto voiceAgentId : inactiveVoiceAgentIds) {
        auto voiceAgentIt = mVoiceAgents.find(voiceAgentId);
        if (voiceAgentIt != mVoiceAgents.end()) {
            ++agentsDeactivated;
            if (voiceAgentIt->second->isActive()) {
                // deactivate the voiceagent
                voiceAgentIt->second->setIsActive(false);
                // Notify observers
                for (auto observer : mVoiceAgentChangeObservers) {
                    observer->OnVoiceAgentDeactivated(voiceAgentIt->second);
                }
            }
        }
    }

    return agentsDeactivated;
}

bool VoiceAgentsDataManager::setDefaultVoiceAgent(const string& voiceAgentId) {
    if (mVoiceAgents.empty() || voiceAgentId.empty()) {
        string message = string("Failed to set default voiceagent id: ") + voiceAgentId;
        mLogger->log(Level::ERROR, TAG, message);
        return false;
    }

    auto defaultVoiceAgentIt = mVoiceAgents.find(voiceAgentId);

    if (defaultVoiceAgentIt != mVoiceAgents.end()) {
        if (mDefaultVoiceAgentId != voiceAgentId) {
            // Notify observers
            for (auto observer : mVoiceAgentChangeObservers) {
                observer->OnDefaultVoiceAgentChanged(defaultVoiceAgentIt->second);
            }
        }
        mDefaultVoiceAgentId = voiceAgentId;
    } else {
        mLogger->log(Level::ERROR, TAG, "Can't set default agent. Invalid voice agent id:" + voiceAgentId);
        return false;
    }

    return true;
}

std::string VoiceAgentsDataManager::getDefaultVoiceAgent() {
    return mDefaultVoiceAgentId;
}

bool VoiceAgentsDataManager::setActiveWakeWord(const string& voiceAgentId, const string& wakeword) {
    if (mVoiceAgents.empty() || wakeword.empty()) {
        string message =
            string("Failed to set active wakeword: ") + wakeword + string(" for voiceagent id: ") + voiceAgentId;
        mLogger->log(Level::ERROR, TAG, message);
        return false;
    }

    auto voiceAgentIt = mVoiceAgents.find(voiceAgentId);
    if (voiceAgentIt == mVoiceAgents.end()) {
        return false;
    }

    string oldWakeWord = voiceAgentIt->second->getActiveWakeword();
    if (oldWakeWord != wakeword) {
        voiceAgentIt->second->setActiveWakeWord(wakeword);
        // Notify observers
        for (auto observer : mVoiceAgentChangeObservers) {
            observer->OnVoiceAgentActiveWakeWordChanged(voiceAgentIt->second);
        }
    }

    return true;
}

bool VoiceAgentsDataManager::addNewVoiceAgent(
    const string& id,
    const string& name,
    const string& description,
    const string& api,
    const string& vendor,
    const string& activeWakeword,
    const bool isActive,
    const shared_ptr<unordered_set<string>> wakewords) {
    shared_ptr<VoiceAgent> voiceAgent =
        VoiceAgent::create(mLogger, id, name, description, api, vendor, activeWakeword, isActive, wakewords);

    if (voiceAgent.get() == nullptr || voiceAgent->getId().empty()) {
        string message = string("Invalid Arguments: Failed to add new voiceagent");
        mLogger->log(Level::ERROR, TAG, message);
        return false;
    }

    if (!mVoiceAgents.empty() && mVoiceAgents.find(voiceAgent->getId()) != mVoiceAgents.end()) {
        string message =
            string("Failed to add new voiceagent. Voiceagent: ") + voiceAgent->getId() + string(" already exists.");
        mLogger->log(Level::ERROR, TAG, message);
        return false;
    }

    mVoiceAgents.insert(make_pair(voiceAgent->getId(), voiceAgent));

    // Notify the observers
    for (auto observer : mVoiceAgentChangeObservers) {
        observer->OnVoiceAgentAdded(voiceAgent);
    }

    // Create all vshl events for the voiceagent.
    mVoiceAgentEventsHandler->createVshlEventsForVoiceAgent(voiceAgent->getId());

    return true;
}

void VoiceAgentsDataManager::startNewSubscriptionProcess() {
    if (mAlreadyPeformedSubscriptions)
        return;

    for (auto agent : mVoiceAgents) {
        // Temporarily added to trigger the voiceagent to start the capability
        // subscription process.
        callStartSubscriptionProcessAPI(agent.second);
        mAlreadyPeformedSubscriptions = true;
    }

}

void VoiceAgentsDataManager::callStartSubscriptionProcessAPI(const shared_ptr<VoiceAgent> voiceAgent) {
    if (!voiceAgent) {
        mLogger->log(Level::ERROR, TAG, "Failed to callStartSubscriptionProcessAPI. Invalid input parameter.");
        return;
    }

    if (!mAfbApi) {
        mLogger->log(
            Level::ERROR, TAG, "Failed to mAfbApi on voicegent: " + voiceAgent->getId() + ", No API.");
        return;
    }

    // TODO(Naveen): Move to utilities.
    json_object* object = NULL;
    std::string error, info;
    int rc = mAfbApi->callSync(voiceAgent->getApi(), VA_VERB_START_SUBSCRIPTION_PROCESS, NULL, &object, error, info);

    if (object) {
        free(object);
    }

    return;
}

bool VoiceAgentsDataManager::removeVoiceAgent(const string& voiceAgentId) {
    if (mVoiceAgents.empty()) {
        string message = string("Failed to remove voiceagent: ") + voiceAgentId + string(". Voiceagents data empty.");
        mLogger->log(Level::ERROR, TAG, message);
        return false;
    }

    auto voiceAgentIt = mVoiceAgents.find(voiceAgentId);
    if (voiceAgentIt == mVoiceAgents.end()) {
        string message = string("Failed to remove voiceagent: ") + voiceAgentId + string(". Doesn't exist.");
        mLogger->log(Level::ERROR, TAG, message);
        return false;
    }

    auto voiceAgent = voiceAgentIt->second;
    // Remove from the map
    mVoiceAgents.erase(voiceAgentId);
    // Notify the observers
    for (auto observer : mVoiceAgentChangeObservers) {
        observer->OnVoiceAgentRemoved(voiceAgent);
    }

    // Remove all vshl events for the voiceagent.
    mVoiceAgentEventsHandler->removeVshlEventsForVoiceAgent(voiceAgent->getId());

    return true;
}

std::set<std::shared_ptr<vshl::common::interfaces::IVoiceAgent>> VoiceAgentsDataManager::getAllVoiceAgents() {
    std::set<std::shared_ptr<vshl::common::interfaces::IVoiceAgent>> voiceAgentsSet;
    for (auto element : mVoiceAgents) {
        voiceAgentsSet.insert(element.second);
    }

    return voiceAgentsSet;
}

// Returns the event filter that belongs to the core module.
shared_ptr<vshl::common::interfaces::IEventFilter> VoiceAgentsDataManager::getEventFilter() const {
    return mVoiceAgentEventsHandler;
}

bool VoiceAgentsDataManager::subscribeToVshlEventFromVoiceAgent(
    vshl::common::interfaces::IAFBRequest& request,
    const string eventName,
    const string voiceAgentId) {
    auto voiceAgentIt = mVoiceAgents.find(voiceAgentId);
    if (voiceAgentIt == mVoiceAgents.end()) {
        mLogger->log(
            Level::ERROR,
            TAG,
            "\
          Failed to subscribe to VSHL events from voiceagent. VoiceAgent: " +
                voiceAgentId + " doesn't exist.");
        return false;
    }
    return mVoiceAgentEventsHandler->subscribeToVshlEventFromVoiceAgent(request, eventName, voiceAgentIt->second);
}

bool VoiceAgentsDataManager::addVoiceAgentsChangeObserver(
    shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver> observer) {
    if (!observer) {
        return false;
    }

    mVoiceAgentChangeObservers.insert(observer);
    return true;
}

bool VoiceAgentsDataManager::removeVoiceAgentsChangeObserver(
    shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver> observer) {
    if (!observer) {
        return false;
    }

    mVoiceAgentChangeObservers.erase(observer);
    return true;
}
}  // namespace voiceagents
}  // namespace vshl
