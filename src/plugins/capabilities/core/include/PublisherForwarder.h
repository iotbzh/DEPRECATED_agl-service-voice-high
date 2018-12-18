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
#ifndef VSHL_CAPABILITIES_CORE_PUBLISHERFORWARDER_H_
#define VSHL_CAPABILITIES_CORE_PUBLISHERFORWARDER_H_

#include <memory>

#include "capabilities/core/include/SubscriberForwarder.h"

#include "interfaces/afb/IAFBApi.h"
#include "interfaces/capabilities/ICapability.h"
#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace capabilities {
namespace core {
/*
 * This class is responsible for forwarding the messages to be published
 * to subscriber forwarder. Subscriber forwarder will deliver the messages
 * as AFB Events to all the subscribed clients.
 * There is one PublisherForwarder and one SubscriberForwarder per capability.
 */
class PublisherForwarder {
public:
    // Create a PublisherForwarder.
    static std::shared_ptr<PublisherForwarder> create(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::ICapability> capability);

    // Connect a subscriber forwarder to this publisher forwarder
    void setSubscriberForwarder(shared_ptr<SubscriberForwarder> subscriberForwarder);

    // Forward message to the subscriber forwarder
    bool forwardMessage(const string action, const string payload);

    // Destructor
    ~PublisherForwarder();

private:
    // Constructor
    PublisherForwarder(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::ICapability> capability);

    // Subscriber forwarder connected to this publisher forwarder.
    shared_ptr<SubscriberForwarder> mSubscriberForwarder;

    // Capability
    shared_ptr<vshl::common::interfaces::ICapability> mCapability;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace core
}  // namespace capabilities
}  // namespace vshl

#endif  // VSHL_CAPABILITIES_CORE_PUBLISHERFORWARDER_H_
