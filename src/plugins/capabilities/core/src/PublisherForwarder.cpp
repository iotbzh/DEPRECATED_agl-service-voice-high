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
#include "capabilities/core/include/PublisherForwarder.h"

static string TAG = "vshl::capabilities::PublisherForwarder";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace capabilities {
namespace core {

// Create a PublisherForwarder.
std::shared_ptr<PublisherForwarder> PublisherForwarder::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::ICapability> capability) {
    if (logger == nullptr) {
        return nullptr;
    }

    if (capability == nullptr) {
        logger->log(Level::ERROR, TAG, "Failed to create PublisherForwarder: Capability null");
        return nullptr;
    }

    auto publisherForwarder = std::shared_ptr<PublisherForwarder>(new PublisherForwarder(logger, capability));
    return publisherForwarder;
}

// Constructor
PublisherForwarder::PublisherForwarder(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::ICapability> capability) {
    mCapability = capability;
    mLogger = logger;
}

// Destructor
PublisherForwarder::~PublisherForwarder() {
}

void PublisherForwarder::setSubscriberForwarder(shared_ptr<SubscriberForwarder> subscriberForwarder) {
    mSubscriberForwarder = subscriberForwarder;
}

bool PublisherForwarder::forwardMessage(const string action, const string payload) {
    if (!mSubscriberForwarder) {
        mLogger->log(Level::ERROR, TAG, "Failed to forward message for capability: " + mCapability->getName());
        return false;
    }

    return mSubscriberForwarder->forwardMessage(action, payload);
}

}  // namespace core
}  // namespace capabilities
}  // namespace vshl
