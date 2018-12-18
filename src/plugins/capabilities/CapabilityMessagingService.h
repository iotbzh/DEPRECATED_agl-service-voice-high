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
#ifndef VSHL_CAPABILITIES_CAPABILITYMESSAGINGSERVICE_H_
#define VSHL_CAPABILITIES_CAPABILITYMESSAGINGSERVICE_H_

#include <memory>
#include <string>
#include <unordered_map>

#include "capabilities/core/include/MessageChannel.h"
#include "interfaces/afb/IAFBApi.h"
#include "interfaces/capabilities/ICapability.h"
#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace capabilities {
/*
 * This hosts service APIs that clients can use to subscribe and
 * forward capability messages. Each capability has a name and
 * direction (upstream/downstream). Upstream messages are from
 * voiceagents to Apps and downstream messages are Apps to voiceagents.
 * This class will use a factory to create publisher and subcribers for
 * each capability and create assiociations between them.
 */
class CapabilityMessagingService {
public:
  // Create a CapabilityMessagingService.
  static std::unique_ptr<CapabilityMessagingService>
  create(shared_ptr<vshl::common::interfaces::ILogger> logger,
         shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

  // Subscribe to capability specific messages.
  bool subscribe(vshl::common::interfaces::IAFBRequest &request,
                 shared_ptr<common::interfaces::ICapability> capability,
                 const string action);

  // Publish capability messages.
  bool publish(shared_ptr<common::interfaces::ICapability> capability,
               const string action, const string payload);

  // Destructor
  ~CapabilityMessagingService();

private:
  // Constructor
  CapabilityMessagingService(
      shared_ptr<vshl::common::interfaces::ILogger> logger,
      shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

  // Binding API reference
  shared_ptr<vshl::common::interfaces::IAFBApi> mAfbApi;

  // Create a message channel for the capability.
  shared_ptr<vshl::capabilities::core::MessageChannel>
  getMessageChannel(shared_ptr<common::interfaces::ICapability> capability);

  // Map of capabilities to message channels.
  unordered_map<string, shared_ptr<vshl::capabilities::core::MessageChannel>>
      mMessageChannelsMap;

  // Logger
  shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

} // namespace capabilities
} // namespace vshl

#endif // VSHL_CAPABILITIES_CAPABILITYMESSAGINGSERVICE_H_
