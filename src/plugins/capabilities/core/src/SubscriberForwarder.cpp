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
#include "capabilities/core/include/SubscriberForwarder.h"

static string TAG = "vshl::capabilities::SubscriberForwarder";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace capabilities {
namespace core {

// Create a SubscriberForwarder.
std::shared_ptr<SubscriberForwarder> SubscriberForwarder::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
    shared_ptr<vshl::common::interfaces::ICapability> capability) {
    if (logger == nullptr) {
        return nullptr;
    }

    if (afbApi == nullptr) {
        logger->log(Level::ERROR, TAG, "Failed to create SubscriberForwarder: AFB API null");
        return nullptr;
    }

    if (capability == nullptr) {
        logger->log(Level::ERROR, TAG, "Failed to create SubscriberForwarder: Capability null");
        return nullptr;
    }

    auto subscriberForwarder =
        std::shared_ptr<SubscriberForwarder>(new SubscriberForwarder(logger, afbApi, capability));
    return subscriberForwarder;
}

SubscriberForwarder::SubscriberForwarder(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
    shared_ptr<vshl::common::interfaces::ICapability> capability) :
        mAfbApi(afbApi),
        mLogger(logger),
        mCapability(capability) {
    createEvents();
}

SubscriberForwarder::~SubscriberForwarder() {
    mUpstreamEventsMap.clear();
    mDownstreamEventsMap.clear();
}

void SubscriberForwarder::createEvents() {
    if (!mCapability) {
        mLogger->log(Level::NOTICE, TAG, "Create Events failed. No capability assigned.");
        return;
    }

    // Upstream events
    auto upstreamEvents = mCapability->getUpstreamMessages();
    for (auto upstreamEventName : upstreamEvents) {
        auto it = mUpstreamEventsMap.find(upstreamEventName);
        if (it == mUpstreamEventsMap.end() && mAfbApi) {
            // create a new event and add it to the map.
            shared_ptr<common::interfaces::IAFBApi::IAFBEvent> event = mAfbApi->createEvent(upstreamEventName);
            if (event == nullptr) {
                mLogger->log(Level::ERROR, TAG, "Failed to create upstream event: " + upstreamEventName);
            } else {
                mUpstreamEventsMap.insert(make_pair(upstreamEventName, event));
            }
        }
    }

    // Downstream events
    auto downstreamEvents = mCapability->getDownstreamMessages();
    for (auto downstreamEventName : downstreamEvents) {
        auto it = mDownstreamEventsMap.find(downstreamEventName);
        if (it == mDownstreamEventsMap.end() && mAfbApi) {
            // create a new event and add it to the map.
            shared_ptr<common::interfaces::IAFBApi::IAFBEvent> event = mAfbApi->createEvent(downstreamEventName);
            if (event == nullptr) {
                mLogger->log(Level::ERROR, TAG, "Failed to create downstream event: " + downstreamEventName);
            } else {
                mDownstreamEventsMap.insert(make_pair(downstreamEventName, event));
            }
        }
    }
}

bool SubscriberForwarder::forwardMessage(const string action, const string payload) {
    auto upstreamEventIt = mUpstreamEventsMap.find(action);
    if (upstreamEventIt != mUpstreamEventsMap.end()) {
        mLogger->log(Level::NOTICE, TAG, "Publishing upstream event: " + action);
        upstreamEventIt->second->publishEvent(json_object_new_string(payload.c_str()));
        // Let the capability know about it.
        mCapability->onMessagePublished(action);
        return true;
    }

    auto downstreamEventIt = mDownstreamEventsMap.find(action);
    if (downstreamEventIt != mDownstreamEventsMap.end()) {
        mLogger->log(Level::NOTICE, TAG, "Publishing downstream event: " + action);
        downstreamEventIt->second->publishEvent(json_object_new_string(payload.c_str()));
        return true;
    }

    mLogger->log(Level::NOTICE, TAG, "Failed to publish upstream event: " + action);
    return false;
}

bool SubscriberForwarder::subscribe(vshl::common::interfaces::IAFBRequest& request, const string action) {
    auto upstreamEventIt = mUpstreamEventsMap.find(action);
    if (upstreamEventIt != mUpstreamEventsMap.end()) {
        mLogger->log(Level::NOTICE, TAG, "Subscribing to upstream event: " + action);
        return upstreamEventIt->second->subscribe(request);
    }

    auto downstreamEventIt = mDownstreamEventsMap.find(action);
    if (downstreamEventIt != mDownstreamEventsMap.end()) {
        mLogger->log(Level::NOTICE, TAG, "Subscribing to downstream event: " + action);
        return downstreamEventIt->second->subscribe(request);
    }

    mLogger->log(Level::NOTICE, TAG, "Failed to subscribe to upstream event: " + action);
    return false;
}

}  // namespace core
}  // namespace capabilities
}  // namespace vshl
