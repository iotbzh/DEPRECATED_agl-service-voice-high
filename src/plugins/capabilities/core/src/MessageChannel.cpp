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
#include "capabilities/core/include/MessageChannel.h"

namespace vshl {
namespace capabilities {
namespace core {

// Create a MessageChannel.
std::shared_ptr<MessageChannel> MessageChannel::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> api,
    shared_ptr<vshl::common::interfaces::ICapability> capability) {
    auto messageChannel = std::shared_ptr<MessageChannel>(new MessageChannel(logger, api, capability));
    return messageChannel;
}

MessageChannel::MessageChannel(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> api,
    shared_ptr<vshl::common::interfaces::ICapability> capability) {
    // Subscriber forwarder
    mSubscriberForwarder = SubscriberForwarder::create(logger, api, capability);
    // Publisher forwarder
    mPublisherForwarder = PublisherForwarder::create(logger, capability);
    mPublisherForwarder->setSubscriberForwarder(mSubscriberForwarder);
}

bool MessageChannel::publish(const string action, const string payload) {
    return mPublisherForwarder->forwardMessage(action, payload);
}

bool MessageChannel::subscribe(vshl::common::interfaces::IAFBRequest& request, const string action) {
    return mSubscriberForwarder->subscribe(request, action);
}

}  // namespace core
}  // namespace capabilities
}  // namespace vshl
