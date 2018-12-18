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
#ifndef VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTSTATE_EVENT_HANDLER_H_
#define VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTSTATE_EVENT_HANDLER_H_

#include <algorithm>
#include <memory>
#include <unordered_map>

#include "interfaces/afb/IAFBApi.h"
#include "interfaces/utilities/events/IEventFilter.h"
#include "interfaces/utilities/logging/ILogger.h"
#include "voiceagents/VoiceAgentEventNames.h"
#include "voiceagents/include/VoiceAgent.h"

using namespace std;

namespace vshl {
namespace voiceagents {
/*
 * This class is reponsible for handling agent specific events
 * subscription and delivery on behalf of the high level voice service.
 * This class also listen to the incoming events from voice agents
 * and implements propagation to application layer.
 */
class VoiceAgentEventsHandler : public vshl::common::interfaces::IEventFilter {
public:
    // Create a VREventFilter.
    static shared_ptr<VoiceAgentEventsHandler> create(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    // Creates all the vshl events for a specific voiceagent id.
    // For e.g if voiceagent is VA-001 then a new vshl event
    // voice_authstate_event#VA-001 for auth state will be created.
    // Please see VoiceAgentEventNames.h for all the event names.
    void createVshlEventsForVoiceAgent(const string voiceAgentId);

    // Removes the events from its bookkeeping.
    void removeVshlEventsForVoiceAgent(const string voiceAgentId);

    // Subscribe to a vshl event corresponding to a voiceagent.
    bool subscribeToVshlEventFromVoiceAgent(
        vshl::common::interfaces::IAFBRequest& request,
        const string eventName,
        const shared_ptr<VoiceAgent> voiceAgent);

    ~VoiceAgentEventsHandler();

protected:
    string getName() override;

    // IEventFilter override
    bool onIncomingEvent(const string eventName, const string voiceAgentId, const string payload) override;

private:
    // Constructor
    VoiceAgentEventsHandler(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    // Helper method to generate the event name with voiceagent Id
    // concatenated.
    string createEventNameWithVAId(string eventName, string voiceAgentId);

    // call subscribe verb on the voiceagent. True if subscription successful.
    // False otherwise.
    bool callSubscribeVerb(const shared_ptr<VoiceAgent> voiceAgent);

    // Binding API reference
    shared_ptr<vshl::common::interfaces::IAFBApi> mAfbApi;

    // A map of VSHL event ID to its Event object
    unordered_map<string, shared_ptr<common::interfaces::IAFBApi::IAFBEvent>> mEventsMap;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace voiceagents
}  // namespace vshl

#endif  // VSHL_VOICEAGENTS_INCLUDE_VOICEAGENTSTATE_EVENT_HANDLER_H_
