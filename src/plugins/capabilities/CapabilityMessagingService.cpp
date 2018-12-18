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
#include "capabilities/CapabilityMessagingService.h"

#include "capabilities/core/include/PublisherForwarder.h"
#include "capabilities/core/include/SubscriberForwarder.h"

static string TAG = "vshl::capabilities::CapabilityMessagingService";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace capabilities {

// Create a CapabilityMessagingService.
unique_ptr<CapabilityMessagingService> CapabilityMessagingService::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) {
    if (logger == nullptr) {
        return nullptr;
    }

    if (afbApi == nullptr) {
        logger->log(Level::ERROR, TAG, "Failed to create CapabilityMessagingService: AFB API null");
        return nullptr;
    }

    auto capabilityMessageService =
        std::unique_ptr<CapabilityMessagingService>(new CapabilityMessagingService(logger, afbApi));
    return capabilityMessageService;
}

CapabilityMessagingService::~CapabilityMessagingService() {
    mMessageChannelsMap.clear();
}

CapabilityMessagingService::CapabilityMessagingService(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) :
        mAfbApi(afbApi),
        mLogger(logger) {
}

// Subscribe to capability specific messages.
bool CapabilityMessagingService::subscribe(
    vshl::common::interfaces::IAFBRequest& request,
    shared_ptr<common::interfaces::ICapability> capability,
    const string action) {
    auto capabilityName = capability->getName();

    if (capabilityName.empty()) {
        mLogger->log(Level::ERROR, TAG, "Failed to subscribe to message. Invalid input.");
        return false;
    }

    auto messageChannel = getMessageChannel(capability);
    return messageChannel->subscribe(request, action);
}

// Publish capability messages.
bool CapabilityMessagingService::publish(
    shared_ptr<common::interfaces::ICapability> capability,
    const string action,
    const string payload) {
    auto capabilityName = capability->getName();

    if (capabilityName.empty()) {
        mLogger->log(Level::ERROR, TAG, "Failed to publish message. Invalid input.");
        return false;
    }

    auto messageChannelIt = mMessageChannelsMap.find(capabilityName);
    if (messageChannelIt == mMessageChannelsMap.end()) {
        mLogger->log(
            Level::ERROR,
            TAG,
            "Failed to publish message. Message channel doesn't exist for capability " + capabilityName);
        return false;
    }

    return messageChannelIt->second->publish(action, payload);
}

shared_ptr<vshl::capabilities::core::MessageChannel> CapabilityMessagingService::getMessageChannel(
    shared_ptr<common::interfaces::ICapability> capability) {
    auto capabilityName = capability->getName();

    if (capabilityName.empty()) {
        mLogger->log(Level::ERROR, TAG, "Failed to create message channel. Invalid input.");
        return nullptr;
    }

    auto messageChannelIt = mMessageChannelsMap.find(capabilityName);
    if (messageChannelIt == mMessageChannelsMap.end()) {
        mLogger->log(Level::INFO, TAG, "Creating new message channel for capability: " + capabilityName);
        auto messageChannel = vshl::capabilities::core::MessageChannel::create(mLogger, mAfbApi, capability);
        mMessageChannelsMap.insert(make_pair(capabilityName, messageChannel));
        return messageChannel;
    }

    return messageChannelIt->second;
}

}  // namespace capabilities
}  // namespace vshl
