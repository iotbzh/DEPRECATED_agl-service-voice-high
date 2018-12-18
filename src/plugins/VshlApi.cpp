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
#include "VshlApi.h"

#include <list>

#include "afb/AFBApiImpl.h"
#include "afb/AFBRequestImpl.h"
#include "appmanagement/AppController.h"
#include "capabilities/CapabilitiesFactory.h"
#include "capabilities/CapabilityMessagingService.h"
#include "core/VRRequestProcessor.h"
#include "utilities/events/EventRouter.h"
#include "utilities/logging/Logger.h"
#include "voiceagents/VoiceAgentEventNames.h"
#include "voiceagents/VoiceAgentsDataManager.h"

#include "json.hpp"

using namespace std;

CTLP_CAPI_REGISTER("vshl-api");

static std::string TAG = "vshl::plugins::VshlAPI";

static std::string VA_JSON_ATTR_DEFAULT = "default";
static std::string VA_JSON_ATTR_AGENTS = "agents";
static std::string VA_JSON_ATTR_ID = "id";
static std::string VA_JSON_ATTR_NAME = "name";
static std::string VA_JSON_ATTR_API = "api";
static std::string VA_JSON_ATTR_ACTIVE = "active";
static std::string VA_JSON_ATTR_WWS = "wakewords";
static std::string VA_JSON_ATTR_ACTIVE_WW = "activewakeword";
static std::string VA_JSON_ATTR_DESCRIPTION = "description";
static std::string VA_JSON_ATTR_VENDOR = "vendor";

static std::string STARTLISTENING_JSON_ATTR_REQUEST = "request_id";

static std::string EVENTS_JSON_ATTR_VA_ID = "va_id";
static std::string EVENTS_JSON_ATTR_EVENTS = "events";

static std::string CAPABILITIES_JSON_ATTR_ACTION = "action";
static std::string CAPABILITIES_JSON_ATTR_ACTIONS = "actions";
static std::string CAPABILITIES_JSON_ATTR_PAYLOAD = "payload";

static std::shared_ptr<vshl::utilities::logging::Logger> sLogger;
static std::shared_ptr<vshl::common::interfaces::IAFBApi> sAfbApi;
static std::shared_ptr<vshl::appmanagement::AppController> sAppController;
static std::unique_ptr<vshl::capabilities::CapabilitiesFactory> sCapabilitiesFactory;
static std::unique_ptr<vshl::capabilities::CapabilityMessagingService> sCapabilityMessagingService;
static std::unique_ptr<vshl::core::VRRequestProcessor> sVRRequestProcessor;
static std::unique_ptr<vshl::voiceagents::VoiceAgentsDataManager> sVoiceAgentsDataManager;
static std::unique_ptr<vshl::utilities::events::EventRouter> sEventRouter;

using json = nlohmann::json;
using Level = vshl::utilities::logging::Logger::Level;

CTLP_ONLOAD(plugin, ret) {
    if (plugin->api == nullptr) {
        return -1;
    }

    // Logger
    sLogger = vshl::utilities::logging::Logger::create(plugin->api);
    // sLogger->log(Level::INFO, TAG, "Vshl plugin loaded & initialized.");

    // AFB Wrapper
    sAfbApi = vshl::afb::AFBApiImpl::create(plugin->api);

    // AppController
    sAppController = vshl::appmanagement::AppController::create(sLogger, sAfbApi);
    if (!sAppController) {
        sLogger->log(Level::ERROR, TAG, "Failed to create AppController");
        return -1;
    }

    // VRRequestProcessor
    auto vrRequestProcessorDelegate = vshl::core::VRRequestProcessorDelegate::create(sLogger, sAfbApi);
    sVRRequestProcessor = vshl::core::VRRequestProcessor::create(sLogger, vrRequestProcessorDelegate);
    if (!sVRRequestProcessor) {
        sLogger->log(Level::ERROR, TAG, "Failed to create VRRequestProcessor");
        return -1;
    }

    // VoiceAgentDataManager
    sVoiceAgentsDataManager = vshl::voiceagents::VoiceAgentsDataManager::create(sLogger, sAfbApi);
    if (!sVoiceAgentsDataManager) {
        sLogger->log(Level::ERROR, TAG, "Failed to create VoiceAgentsDataManager");
        return -1;
    }
    sVoiceAgentsDataManager->addVoiceAgentsChangeObserver(sVRRequestProcessor->getVoiceAgentsChangeObserver());

    // EventRouter
    sEventRouter = vshl::utilities::events::EventRouter::create(sLogger);
    if (!sEventRouter) {
        sLogger->log(Level::ERROR, TAG, "Failed to create EventRouter");
        return -1;
    }
    sEventRouter->addEventFilter(sVoiceAgentsDataManager->getEventFilter());

    sCapabilitiesFactory = vshl::capabilities::CapabilitiesFactory::create(sAppController, sLogger);
    if (!sCapabilitiesFactory) {
        sLogger->log(Level::ERROR, TAG, "Failed to create CapabilitiesFactory");
        return -1;
    }

    sCapabilityMessagingService = vshl::capabilities::CapabilityMessagingService::create(sLogger, sAfbApi);
    if (!sCapabilityMessagingService) {
        sLogger->log(Level::ERROR, TAG, "Failed to create CapabilityMessagingService");
        return -1;
    }

    return 0;
}

CTLP_CAPI(onAuthStateEvent, source, argsJ, eventJ) {
    if (sEventRouter == nullptr) {
        return -1;
    }

    string eventName = vshl::voiceagents::VSHL_EVENT_AUTH_STATE_EVENT;
    json eventJson = json::parse(json_object_to_json_string(eventJ));
    if (eventJson.find(EVENTS_JSON_ATTR_VA_ID) == eventJson.end()) {
        sLogger->log(Level::ERROR, TAG, "onAuthStateEvent: No voiceagent id found.");
        return -1;
    }
    std::string voiceAgentId(eventJson[EVENTS_JSON_ATTR_VA_ID].get<string>());

    if (!sEventRouter->handleIncomingEvent(eventName, voiceAgentId, json_object_to_json_string(eventJ))) {
        sLogger->log(Level::ERROR, TAG, "onAuthStateEvent: Failed to handle.");
        return -1;
    }

    return 0;
}

CTLP_CAPI(onConnectionStateEvent, source, argsJ, eventJ) {
    if (sEventRouter == nullptr) {
        return -1;
    }

    string eventName = vshl::voiceagents::VSHL_EVENT_CONNECTION_STATE_EVENT;
    json eventJson = json::parse(json_object_to_json_string(eventJ));
    if (eventJson.find(EVENTS_JSON_ATTR_VA_ID) == eventJson.end()) {
        sLogger->log(Level::ERROR, TAG, "onConnectionStateEvent: No voiceagent id found.");
        return -1;
    }
    std::string voiceAgentId(eventJson[EVENTS_JSON_ATTR_VA_ID].get<string>());

    if (!sEventRouter->handleIncomingEvent(eventName, voiceAgentId, json_object_to_json_string(eventJ))) {
        sLogger->log(Level::ERROR, TAG, "onConnectionStateEvent: Failed to handle.");
        return -1;
    }

    return 0;
}

CTLP_CAPI(onDialogStateEvent, source, argsJ, eventJ) {
    if (sEventRouter == nullptr) {
        return -1;
    }

    string eventName = vshl::voiceagents::VSHL_EVENT_DIALOG_STATE_EVENT;
    json eventJson = json::parse(json_object_to_json_string(eventJ));
    if (eventJson.find(EVENTS_JSON_ATTR_VA_ID) == eventJson.end()) {
        sLogger->log(Level::ERROR, TAG, "onDialogStateEvent: No voiceagent id found.");
        return -1;
    }
    std::string voiceAgentId(eventJson[EVENTS_JSON_ATTR_VA_ID].get<string>());

    if (!sEventRouter->handleIncomingEvent(eventName, voiceAgentId, json_object_to_json_string(eventJ))) {
        sLogger->log(Level::ERROR, TAG, "onDialogStateEvent: Failed to handle.");
        return -1;
    }

    return 0;
}

CTLP_CAPI(loadVoiceAgentsConfig, source, argsJ, eventJ) {
    if (sVoiceAgentsDataManager == nullptr) {
        sLogger->log(Level::WARNING, TAG, "loadVoiceAgentsConfig: Voice service not initialized.");
        return -1;
    }

    if (argsJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "loadVoiceAgentsConfig: No arguments supplied.");
        return -1;
    }

    json agentsConfigJson = json::parse(json_object_to_json_string(argsJ));
    if (agentsConfigJson.find(VA_JSON_ATTR_AGENTS) == agentsConfigJson.end()) {
        sLogger->log(Level::ERROR, TAG, "loadVoiceAgentsConfig: No agents object found in agents json");
        return -1;
    }

    json agentsJson = agentsConfigJson[VA_JSON_ATTR_AGENTS];
    for (auto agentIt = agentsJson.begin(); agentIt != agentsJson.end(); ++agentIt) {
        json agentJson = *agentIt;

        if (agentJson.find(VA_JSON_ATTR_ID) == agentJson.end() ||
            agentJson.find(VA_JSON_ATTR_ACTIVE) == agentJson.end() ||
            agentJson.find(VA_JSON_ATTR_NAME) == agentJson.end() ||
            agentJson.find(VA_JSON_ATTR_API) == agentJson.end() ||
            agentJson.find(VA_JSON_ATTR_WWS) == agentJson.end() ||
            agentJson.find(VA_JSON_ATTR_ACTIVE_WW) == agentJson.end() ||
            agentJson.find(VA_JSON_ATTR_DESCRIPTION) == agentJson.end() ||
            agentJson.find(VA_JSON_ATTR_VENDOR) == agentJson.end()) {
            std::stringstream error;
            error << "loadVoiceAgentsConfig: One or more missing params in agent "
                     "config "
                  << agentJson.dump();
            sLogger->log(Level::WARNING, TAG, error.str().c_str());
            continue;
        }

        std::string id(agentJson[VA_JSON_ATTR_ID].get<string>());
        std::string name(agentJson[VA_JSON_ATTR_NAME].get<string>());
        std::string api(agentJson[VA_JSON_ATTR_API].get<string>());
        std::string description(agentJson[VA_JSON_ATTR_DESCRIPTION].get<string>());
        std::string vendor(agentJson[VA_JSON_ATTR_VENDOR].get<string>());
        std::string activeWakeword(agentJson[VA_JSON_ATTR_ACTIVE_WW].get<string>());
        bool isActive(agentJson[VA_JSON_ATTR_ACTIVE].get<bool>());

        shared_ptr<unordered_set<string>> wakewords = std::make_shared<unordered_set<string>>();
        json wakewordsJson = agentJson[VA_JSON_ATTR_WWS];
        for (auto wwIt = wakewordsJson.begin(); wwIt != wakewordsJson.end(); ++wwIt) {
            wakewords->insert(wwIt->get<string>());
        }

        sVoiceAgentsDataManager->addNewVoiceAgent(
            id, name, description, api, vendor, activeWakeword, isActive, wakewords);
    }

    // Set the default agent.
    if (agentsConfigJson.find(VA_JSON_ATTR_DEFAULT) == agentsConfigJson.end()) {
        sLogger->log(Level::ERROR, TAG, "loadVoiceAgentsConfig: No default agent found in agents json");
        return -1;
    }
    std::string defaultAgentId(agentsConfigJson[VA_JSON_ATTR_DEFAULT].get<string>());
    sVoiceAgentsDataManager->setDefaultVoiceAgent(defaultAgentId);

    return 0;
}

CTLP_CAPI(startListening, source, argsJ, eventJ) {
    if (sVoiceAgentsDataManager == nullptr) {
        return -1;
    }

    // Start the new subscription process. It will be ignored if already done.
    sVoiceAgentsDataManager->startNewSubscriptionProcess();

    if (sVRRequestProcessor == nullptr) {
        return -1;
    }

    int result = 0;
    string requestId = sVRRequestProcessor->startListening();

    if (!requestId.empty()) {
        json responseJson;
        responseJson[STARTLISTENING_JSON_ATTR_REQUEST] = requestId;
        AFB_ReqSuccess(source->request, json_tokener_parse(responseJson.dump().c_str()), NULL);
    } else {
        AFB_ReqFail(source->request, NULL, "Failed to startListening...");
    }

    return 0;
}

CTLP_CAPI(cancelListening, source, argsJ, eventJ) {
    return 0;
}

CTLP_CAPI(enumerateVoiceAgents, source, argsJ, eventJ) {
    if (sVoiceAgentsDataManager == nullptr) {
        return -1;
    }
    
    auto agents = sVoiceAgentsDataManager->getAllVoiceAgents();
    std::string defaultAgentId(sVoiceAgentsDataManager->getDefaultVoiceAgent());

    json responseJson;
    json agentsJson = json::array();

    for (auto agent : agents) {
        json agentJson;
        agentJson[VA_JSON_ATTR_ID] = agent->getId();
        agentJson[VA_JSON_ATTR_NAME] = agent->getName();
        agentJson[VA_JSON_ATTR_DESCRIPTION] = agent->getDescription();
        agentJson[VA_JSON_ATTR_API] = agent->getApi();
        agentJson[VA_JSON_ATTR_VENDOR] = agent->getVendor();
        agentJson[VA_JSON_ATTR_ACTIVE] = agent->isActive();
        agentJson[VA_JSON_ATTR_ACTIVE_WW] = agent->getActiveWakeword();

        auto wakewords = agent->getWakeWords();
        if (wakewords != nullptr) {
            json wakewordsJson;
            for (auto wakeword : *wakewords) {
                wakewordsJson.push_back(wakeword);
            }
            agentJson[VA_JSON_ATTR_WWS] = wakewordsJson;
        }

        agentsJson.push_back(agentJson);
    }

    responseJson[VA_JSON_ATTR_AGENTS] = agentsJson;
    responseJson[VA_JSON_ATTR_DEFAULT] = defaultAgentId;

    AFB_ReqSuccess(source->request, json_tokener_parse(responseJson.dump().c_str()), NULL);

    return 0;
}

CTLP_CAPI(subscribe, source, argsJ, eventJ) {
    if (sVoiceAgentsDataManager == nullptr) {
        return -1;
    }

    // Start the new subscription process. It will be ignored if already done.
    sVoiceAgentsDataManager->startNewSubscriptionProcess();

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "subscribe: No arguments supplied.");
        return -1;
    }

    json subscribeJson = json::parse(json_object_to_json_string(eventJ));
    if (subscribeJson.find(EVENTS_JSON_ATTR_VA_ID) == subscribeJson.end()) {
        sLogger->log(Level::ERROR, TAG, "subscribe: No voiceagent id found in subscribe json");
        return -1;
    }
    std::string voiceAgentId(subscribeJson[EVENTS_JSON_ATTR_VA_ID].get<string>());

    if (subscribeJson.find(EVENTS_JSON_ATTR_EVENTS) == subscribeJson.end()) {
        sLogger->log(Level::ERROR, TAG, "subscribe: No events array found in subscribe json");
        return -1;
    }
    list<string> events(subscribeJson[EVENTS_JSON_ATTR_EVENTS].get<list<string>>());

    // Subscribe this client for the listed events.
    auto request = vshl::afb::AFBRequestImpl::create(source->request);
    for (auto event : events) {
        if (!sVoiceAgentsDataManager->subscribeToVshlEventFromVoiceAgent(*request, event, voiceAgentId)) {
            sLogger->log(Level::ERROR, TAG, "subscribe: Failed to subscribe to event: " + event);
            return -1;
        }
    }

    AFB_ReqSuccess(source->request, json_object_new_string("Subscription to events successfully completed."), NULL);

    return 0;
}

CTLP_CAPI(setDefaultVoiceAgent, source, argsJ, eventJ) {
    if (sVoiceAgentsDataManager == nullptr) {
        return -1;
    }

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "setDefaultVoiceAgent: No arguments supplied.");
        return -1;
    }

    json jsonRequest = json::parse(json_object_to_json_string(eventJ));
    if (jsonRequest.find(VA_JSON_ATTR_ID) == jsonRequest.end()) {
        sLogger->log(Level::ERROR, TAG, "setDefaultVoiceAgent: voice agent id not found in request json");
        return -1;
    }

    std::string voiceAgentId(jsonRequest[VA_JSON_ATTR_ID].get<string>());
    if (!sVoiceAgentsDataManager->setDefaultVoiceAgent(voiceAgentId)) {
        sLogger->log(Level::ERROR, TAG, "setDefaultVoiceAgent: Failed to set default agent");
        return -1;
    }

    AFB_ReqSuccess(source->request, NULL, NULL);
    return 0;
}

CTLP_CAPI(guiMetadataSubscribe, source, argsJ, eventJ) {
    if (sCapabilitiesFactory == nullptr || sCapabilityMessagingService == nullptr) {
        return -1;
    }

    shared_ptr<vshl::common::interfaces::ICapability> guMetadataCapability = sCapabilitiesFactory->getGuiMetadata();
    if (!guMetadataCapability) {
        sLogger->log(
            Level::WARNING,
            TAG,
            "guimetadataSubscribe: Failed to "
            "fetch guimetadata capability "
            "object.");
        return -1;
    }

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "guimetadataSubscribe: No arguments supplied.");
        return -1;
    }

    json subscribeJson = json::parse(json_object_to_json_string(eventJ));
    if (subscribeJson.find(CAPABILITIES_JSON_ATTR_ACTIONS) == subscribeJson.end()) {
        sLogger->log(Level::ERROR, TAG, "guimetadataSubscribe: No events array found in subscribe json");
        return -1;
    }
    list<string> events(subscribeJson[CAPABILITIES_JSON_ATTR_ACTIONS].get<list<string>>());

    // SUbscribe this client for the guimetadata events.
    auto request = vshl::afb::AFBRequestImpl::create(source->request);
    for (auto event : events) {
        if (!sCapabilityMessagingService->subscribe(*request, guMetadataCapability, event)) {
            sLogger->log(Level::ERROR, TAG, "guimetadataSubscribe: Failed to subscribe to event: " + event);
            return -1;
        }
    }

    AFB_ReqSuccess(
        source->request, json_object_new_string("Subscription to guimetadata events successfully completed."), NULL);
    return 0;
}

CTLP_CAPI(guiMetadataPublish, source, argsJ, eventJ) {
    if (sCapabilitiesFactory == nullptr || sCapabilityMessagingService == nullptr) {
        return -1;
    }

    shared_ptr<vshl::common::interfaces::ICapability> guMetadataCapability = sCapabilitiesFactory->getGuiMetadata();
    if (!guMetadataCapability) {
        sLogger->log(
            Level::WARNING,
            TAG,
            "guimetadataPublish: Failed to fetch "
            "guimetadata capability object.");
        return -1;
    }

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "guimetadataPublish: No arguments supplied.");
        return -1;
    }

    json publishJson = json::parse(json_object_to_json_string(eventJ));
    if (publishJson.find(CAPABILITIES_JSON_ATTR_ACTION) == publishJson.end()) {
        sLogger->log(Level::ERROR, TAG, "guimetadataPublish: No action found in publish json");
        return -1;
    }
    std::string action(publishJson[CAPABILITIES_JSON_ATTR_ACTION].get<string>());

    if (publishJson.find(CAPABILITIES_JSON_ATTR_PAYLOAD) == publishJson.end()) {
        sLogger->log(Level::ERROR, TAG, "guimetadataPublish: No payload found in publish json");
        return -1;
    }
    std::string payload(publishJson[CAPABILITIES_JSON_ATTR_PAYLOAD].get<string>());

    if (!sCapabilityMessagingService->publish(guMetadataCapability, action, payload)) {
        sLogger->log(Level::ERROR, TAG, "guimetadataPublish: Failed to publish message: " + action);
        return -1;
    }

    AFB_ReqSuccess(source->request, json_object_new_string("Successfully published guimetadata messages."), NULL);
    return 0;
}

CTLP_CAPI(phonecontrolSubscribe, source, argsJ, eventJ) {
    if (sCapabilitiesFactory == nullptr || sCapabilityMessagingService == nullptr) {
        return -1;
    }

    shared_ptr<vshl::common::interfaces::ICapability> phoneControlCapability = sCapabilitiesFactory->getPhoneControl();
    if (!phoneControlCapability) {
        sLogger->log(Level::WARNING, TAG, "phoneControlSubscribe: Failed to fetch phone control capability object.");
        return -1;
    }

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "phoneControlSubscribe: No arguments supplied.");
        return -1;
    }

    json subscribeJson = json::parse(json_object_to_json_string(eventJ));
    if (subscribeJson.find(CAPABILITIES_JSON_ATTR_ACTIONS) == subscribeJson.end()) {
        sLogger->log(Level::ERROR, TAG, "phoneControlSubscribe: No events array found in subscribe json");
        return -1;
    }
    list<string> events(subscribeJson[CAPABILITIES_JSON_ATTR_ACTIONS].get<list<string>>());

    // SUbscribe this client for the phone call control events.
    auto request = vshl::afb::AFBRequestImpl::create(source->request);
    for (auto event : events) {
        if (!sCapabilityMessagingService->subscribe(*request, phoneControlCapability, event)) {
            sLogger->log(Level::ERROR, TAG, "phoneControlSubscribe: Failed to subscribe to event: " + event);
            return -1;
        }
    }

    AFB_ReqSuccess(
        source->request, json_object_new_string("Subscription to phone control events successfully completed."), NULL);
    return 0;
}

CTLP_CAPI(phonecontrolPublish, source, argsJ, eventJ) {
    if (sCapabilitiesFactory == nullptr || sCapabilityMessagingService == nullptr) {
        return -1;
    }

    shared_ptr<vshl::common::interfaces::ICapability> phoneControlCapability = sCapabilitiesFactory->getPhoneControl();
    if (!phoneControlCapability) {
        sLogger->log(Level::WARNING, TAG, "phoneControlPublish: Failed to fetch navigation capability object.");
        return -1;
    }

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "phoneControlPublish: No arguments supplied.");
        return -1;
    }

    json publishJson = json::parse(json_object_to_json_string(eventJ));
    if (publishJson.find(CAPABILITIES_JSON_ATTR_ACTION) == publishJson.end()) {
        sLogger->log(Level::ERROR, TAG, "phoneControlPublish: No action found in publish json");
        return -1;
    }
    std::string action(publishJson[CAPABILITIES_JSON_ATTR_ACTION].get<string>());

    if (publishJson.find(CAPABILITIES_JSON_ATTR_PAYLOAD) == publishJson.end()) {
        sLogger->log(Level::ERROR, TAG, "phoneControlPublish: No payload found in publish json");
        return -1;
    }
    std::string payload(publishJson[CAPABILITIES_JSON_ATTR_PAYLOAD].get<string>());

    if (!sCapabilityMessagingService->publish(phoneControlCapability, action, payload)) {
        sLogger->log(Level::ERROR, TAG, "phoneControlPublish: Failed to publish message: " + action);
        return -1;
    }

    AFB_ReqSuccess(source->request, json_object_new_string("Successfully published phone control messages."), NULL);
    return 0;
}

CTLP_CAPI(navigationSubscribe, source, argsJ, eventJ) {
    if (sCapabilitiesFactory == nullptr || sCapabilityMessagingService == nullptr) {
        return -1;
    }

    shared_ptr<vshl::common::interfaces::ICapability> navigationCapability = sCapabilitiesFactory->getNavigation();
    if (!navigationCapability) {
        sLogger->log(Level::WARNING, TAG, "navigationSubscribe: Failed to fetch navigation capability object.");
        return -1;
    }

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "navigationSubscribe: No arguments supplied.");
        return -1;
    }

    json subscribeJson = json::parse(json_object_to_json_string(eventJ));
    if (subscribeJson.find(CAPABILITIES_JSON_ATTR_ACTIONS) == subscribeJson.end()) {
        sLogger->log(Level::ERROR, TAG, "navigationSubscribe: No events array found in subscribe json");
        return -1;
    }
    list<string> events(subscribeJson[CAPABILITIES_JSON_ATTR_ACTIONS].get<list<string>>());

    // SUbscribe this client for the navigation events.
    auto request = vshl::afb::AFBRequestImpl::create(source->request);
    for (auto event : events) {
        if (!sCapabilityMessagingService->subscribe(*request, navigationCapability, event)) {
            sLogger->log(Level::ERROR, TAG, "navigationSubscribe: Failed to subscribe to event: " + event);
            return -1;
        }
    }

    AFB_ReqSuccess(
        source->request, json_object_new_string("Subscription to navigation events successfully completed."), NULL);
    return 0;
}

CTLP_CAPI(navigationPublish, source, argsJ, eventJ) {
    if (sCapabilitiesFactory == nullptr || sCapabilityMessagingService == nullptr) {
        return -1;
    }

    shared_ptr<vshl::common::interfaces::ICapability> navigationCapability = sCapabilitiesFactory->getNavigation();
    if (!navigationCapability) {
        sLogger->log(Level::WARNING, TAG, "navigationPublish: Failed to fetch navigation capability object.");
        return -1;
    }

    if (eventJ == nullptr) {
        sLogger->log(Level::WARNING, TAG, "navigationPublish: No arguments supplied.");
        return -1;
    }

    json publishJson = json::parse(json_object_to_json_string(eventJ));
    if (publishJson.find(CAPABILITIES_JSON_ATTR_ACTION) == publishJson.end()) {
        sLogger->log(Level::ERROR, TAG, "navigationPublish: No action found in publish json");
        return -1;
    }
    std::string action(publishJson[CAPABILITIES_JSON_ATTR_ACTION].get<string>());

    if (publishJson.find(CAPABILITIES_JSON_ATTR_PAYLOAD) == publishJson.end()) {
        sLogger->log(Level::ERROR, TAG, "navigationPublish: No payload found in publish json");
        return -1;
    }
    std::string payload(publishJson[CAPABILITIES_JSON_ATTR_PAYLOAD].get<string>());

    if (!sCapabilityMessagingService->publish(navigationCapability, action, payload)) {
        sLogger->log(Level::ERROR, TAG, "navigationPublish: Failed to publish message: " + action);
        return -1;
    }

    AFB_ReqSuccess(source->request, json_object_new_string("Successfully published navigation messages."), NULL);
    return 0;
}
