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
#ifndef VSHL_CAPABILITIES_CORE_SUBSCRIBERFORWARDER_H_
#define VSHL_CAPABILITIES_CORE_SUBSCRIBERFORWARDER_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "interfaces/afb/IAFBApi.h"
#include "interfaces/capabilities/ICapability.h"
#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace capabilities {
namespace core {
/*
 * This class is responsible for forwarding the messages publishing
 * to the actual clients using AFB.
 */
class SubscriberForwarder {
public:
  // Create a SubscriberForwarder.
  static std::shared_ptr<SubscriberForwarder>
  create(shared_ptr<vshl::common::interfaces::ILogger> logger,
         shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
         shared_ptr<vshl::common::interfaces::ICapability> capability);

  // Publish a capability message to the actual client.
  bool forwardMessage(const string action, const string payload);

  // Subscribe
  bool subscribe(vshl::common::interfaces::IAFBRequest &request,
                 const string action);

  // Destructor
  ~SubscriberForwarder();

private:
  // Constructor
  SubscriberForwarder(
      shared_ptr<vshl::common::interfaces::ILogger> logger,
      shared_ptr<vshl::common::interfaces::IAFBApi> afbApi,
      shared_ptr<vshl::common::interfaces::ICapability> capability);

  // Creates both upstream and downstream events
  void createEvents();

  // Binding API reference
  shared_ptr<vshl::common::interfaces::IAFBApi> mAfbApi;

  // Capability
  shared_ptr<vshl::common::interfaces::ICapability> mCapability;

  // Maps of capability action events to its corresponding Event object.
  // Event name maps to Action Name
  unordered_map<string, shared_ptr<common::interfaces::IAFBApi::IAFBEvent>>
      mUpstreamEventsMap;
  unordered_map<string, shared_ptr<common::interfaces::IAFBApi::IAFBEvent>>
      mDownstreamEventsMap;

  // Logger
  shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

} // namespace core
} // namespace capabilities
} // namespace vshl

#endif // VSHL_CAPABILITIES_CORE_SUBSCRIBERFORWARDER_H_
