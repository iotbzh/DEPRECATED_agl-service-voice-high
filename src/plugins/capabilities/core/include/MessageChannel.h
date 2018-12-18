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
#ifndef VSHL_CAPABILITIES_CORE_MESSAGECHANNEL_H_
#define VSHL_CAPABILITIES_CORE_MESSAGECHANNEL_H_

#include <memory>

#include "capabilities/core/include/PublisherForwarder.h"
#include "capabilities/core/include/SubscriberForwarder.h"
#include "interfaces/afb/IAFBApi.h"
#include "interfaces/capabilities/ICapability.h"
#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace capabilities {
namespace core {
/*
 * MessageChannel has one end as publisher forwarder and the other end
 * as subscriber forwarder.
 */
class MessageChannel {
public:
  // Create a MessageChannel.
  static std::shared_ptr<MessageChannel>
  create(shared_ptr<vshl::common::interfaces::ILogger> logger,
         shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
         shared_ptr<vshl::common::interfaces::ICapability> capability);

  // Sends the message
  bool publish(const string action, const string payload);

  // Subscribe
  bool subscribe(vshl::common::interfaces::IAFBRequest &request,
                 const string action);

  // Destructor
  virtual ~MessageChannel() = default;

private:
  // Constructor
  MessageChannel(shared_ptr<vshl::common::interfaces::ILogger> logger,
                 shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
                 shared_ptr<vshl::common::interfaces::ICapability> capability);

  // Forwarders
  shared_ptr<PublisherForwarder> mPublisherForwarder;
  shared_ptr<SubscriberForwarder> mSubscriberForwarder;
};

} // namespace core
} // namespace capabilities
} // namespace vshl

#endif // VSHL_CAPABILITIES_CORE_MESSAGECHANNEL_H_
