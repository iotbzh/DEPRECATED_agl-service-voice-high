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
#ifndef VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTS_H_
#define VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTS_H_

#include <memory>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include "interfaces/afb/IAFBApi.h"
#include "interfaces/utilities/events/IEventFilter.h"
#include "interfaces/utilities/logging/ILogger.h"
#include "interfaces/voiceagents/IVoiceAgent.h"
#include "interfaces/voiceagents/IVoiceAgentsChangeObserver.h"
#include "voiceagents/include/VoiceAgent.h"
#include "voiceagents/include/VoiceAgentEventsHandler.h"

namespace vshl {
namespace voiceagents {
/*
 * This class implements the data model for voiceagents.
 * Supports add, remove and query operations on voiceagent data.
 * Notifies the observers of the changes in the voiceagents data model.
 */
class VoiceAgentsDataManager {
public:
    // Create a VoiceAgentsDataManager.
    static std::unique_ptr<VoiceAgentsDataManager> create(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    /**
     * Activates the list of voiceagents.
     *
     * @return Number of activated agents
     */
    uint32_t activateVoiceAgents(const unordered_set<string>& activeVoiceAgentIds);

    /**
     * Deactivates the list of voiceagents.
     *
     * @return Number of de-activated agents
     */
    uint32_t deactivateVoiceAgents(const unordered_set<string>& inactiveVoiceAgentIds);

    // Sets the default voiceagent.
    bool setDefaultVoiceAgent(const string& voiceAgentId);

    // Sets the default voiceagent.
    std::string getDefaultVoiceAgent();

    // Sets the active wakeword for the voiceagent.
    bool setActiveWakeWord(const string& voiceAgentId, const string& wakeword);

    // Adds a new voiceagent to the cache and also persists the information in a
    // database.
    // This call would notify all the observers about the new voiceagent addition.
    bool addNewVoiceAgent(
        const string& id,
        const string& name,
        const string& description,
        const string& api,
        const string& vendor,
        const string& activeWakeword,
        const bool isActive,
        const shared_ptr<unordered_set<string>> wakewords);

    // Removes the voiceagent from thecache and also from persistent database.
    // This call would notify all the observers about the removal of the
    // voiceagent.
    bool removeVoiceAgent(const string& voiceAgentId);

    // Returns the set of all voice agents in @c VoiceAgentsDataManger cache
    std::set<std::shared_ptr<vshl::common::interfaces::IVoiceAgent>> getAllVoiceAgents();

    // Returns the event filter that belongs to the core module.
    shared_ptr<vshl::common::interfaces::IEventFilter> getEventFilter() const;

    // Subscribe to an event coming from the voiceagent.
    bool subscribeToVshlEventFromVoiceAgent(
        vshl::common::interfaces::IAFBRequest& request,
        const string eventName,
        const string voiceagentId);

    // Adds a new voiceagent change observer.
    bool addVoiceAgentsChangeObserver(shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver> observer);

    // Removes the  voiceagent change observer from the list.
    bool removeVoiceAgentsChangeObserver(shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver> observer);

    // Trigger the low level voice agents to do their subscriptions.
    void startNewSubscriptionProcess();

    // Destructor
    ~VoiceAgentsDataManager();

private:
    // Constructor
    VoiceAgentsDataManager(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    // Temporarily added to signal the voice agent to start the subscriptionprocess
    // for capability message subscriptions.
    void callStartSubscriptionProcessAPI(const shared_ptr<VoiceAgent> voiceAgent);

    // Binding API reference
    shared_ptr<vshl::common::interfaces::IAFBApi> mAfbApi;

    // A list of all the voiceagent change observers
    unordered_set<shared_ptr<vshl::common::interfaces::IVoiceAgentsChangeObserver>> mVoiceAgentChangeObservers;

    // A map of voiceagents grouped by ID
    unordered_map<string, shared_ptr<VoiceAgent>> mVoiceAgents;

    // Voiceagent event handler.
    shared_ptr<VoiceAgentEventsHandler> mVoiceAgentEventsHandler;

    // Default voiceagent
    string mDefaultVoiceAgentId;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;

    bool mAlreadyPeformedSubscriptions;
};

}  // namespace voiceagents
}  // namespace vshl

#endif  // VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTS_H_
