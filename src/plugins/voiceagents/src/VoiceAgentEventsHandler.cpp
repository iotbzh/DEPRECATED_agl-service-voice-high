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
#include "voiceagents/include/VoiceAgentEventsHandler.h"

static string TAG = "vshl::voiceagents::VoiceAgentEventsHandler";
static string VA_VERB_SUBSCRIBE = "subscribe";

using Level = vshl::common::interfaces::ILogger::Level;
using namespace vshl::common::interfaces;

namespace vshl {
namespace voiceagents {

shared_ptr<VoiceAgentEventsHandler> VoiceAgentEventsHandler::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) {
    auto eventFilter = std::shared_ptr<VoiceAgentEventsHandler>(new VoiceAgentEventsHandler(logger, afbApi));
    return eventFilter;
}

VoiceAgentEventsHandler::VoiceAgentEventsHandler(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) :
        mAfbApi(afbApi),
        mLogger(logger) {
}

VoiceAgentEventsHandler::~VoiceAgentEventsHandler() {
    mEventsMap.clear();
}

string VoiceAgentEventsHandler::getName() {
    return TAG;
}

void VoiceAgentEventsHandler::createVshlEventsForVoiceAgent(const string voiceAgentId) {
    // Update the events map with all the VSHL Events.
    for (auto eventName : VSHL_EVENTS) {
        string eventNameWithVAId = createEventNameWithVAId(eventName, voiceAgentId);
        auto it = mEventsMap.find(eventNameWithVAId);
        if (it == mEventsMap.end() && mAfbApi) {
            // create a new event and add it to the map.
            shared_ptr<IAFBApi::IAFBEvent> event = mAfbApi->createEvent(eventNameWithVAId);
            mEventsMap.insert(make_pair(eventNameWithVAId, event));
        }
    }
}

void VoiceAgentEventsHandler::removeVshlEventsForVoiceAgent(const string voiceAgentId) {
    // Update the events map with all the VSHL Events.
    for (auto eventName : VSHL_EVENTS) {
        string eventNameWithVAId = createEventNameWithVAId(eventName, voiceAgentId);
        auto it = mEventsMap.find(eventNameWithVAId);
        if (it != mEventsMap.end()) {
            mEventsMap.erase(it);
        }
    }
}

bool VoiceAgentEventsHandler::subscribeToVshlEventFromVoiceAgent(
    vshl::common::interfaces::IAFBRequest& request,
    const string eventName,
    const shared_ptr<VoiceAgent> voiceAgent) {
    auto supportedEventsIt = find(VSHL_EVENTS.begin(), VSHL_EVENTS.end(), eventName);
    if (supportedEventsIt == VSHL_EVENTS.end()) {
        mLogger->log(Level::ERROR, TAG, "Event: " + eventName + " not a known event.");
        return false;
    }

    // Check if the entry for the voiceagent is present in the
    // events map. If not then return false because the responsibility
    // of adding to the map lies in the hands of AddVoiceAgent method.
    string eventNameWithVAId = createEventNameWithVAId(eventName, voiceAgent->getId());
    auto createdEventsIt = mEventsMap.find(eventNameWithVAId);
    if (createdEventsIt == mEventsMap.end()) {
        mLogger->log(Level::ERROR, TAG, "Not able to subscribe. Event doesn't exist, " + eventNameWithVAId);
        return false;
    }
    createdEventsIt->second->subscribe(request);

    if (!callSubscribeVerb(voiceAgent)) {
        mLogger->log(Level::WARNING, TAG, "Failed to subscribe to voiceagent: " + voiceAgent->getId());
    }

    return true;
}

// IEventFilter override.
bool VoiceAgentEventsHandler::onIncomingEvent(const string eventName, const string voiceAgentId, const string payload) {
    string eventNameWithVAId = createEventNameWithVAId(eventName, voiceAgentId);
    auto it = mEventsMap.find(eventNameWithVAId);
    if (it != mEventsMap.end()) {
        return it->second->publishEvent(json_object_new_string(payload.c_str()));
    }

    return true;
}

string VoiceAgentEventsHandler::createEventNameWithVAId(string eventName, string voiceAgentId) {
    return eventName + "#" + voiceAgentId;
}

bool VoiceAgentEventsHandler::callSubscribeVerb(const shared_ptr<VoiceAgent> voiceAgent) {
    if (!voiceAgent) {
        mLogger->log(Level::ERROR, TAG, "Failed to callSubscribeVerb. Invalid input parameter.");
        return false;
    }

    if (!mAfbApi) {
        mLogger->log(
            Level::ERROR, TAG, "Failed to callSubscribeVerb on voicegent: " + voiceAgent->getId() + ", No API.");
        return false;
    }

    // TODO(Naveen): Move to utilities.
    json_object* object = NULL;
    std::string error, info;
    int rc = mAfbApi->callSync(voiceAgent->getApi(), VA_VERB_SUBSCRIBE, NULL, &object, error, info);

    if (object) {
        free(object);
    }

    return true;
}
}  // namespace voiceagents
}  // namespace vshl
