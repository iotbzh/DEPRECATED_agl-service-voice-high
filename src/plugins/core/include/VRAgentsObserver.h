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
#ifndef VSHL_CORE_INCLUDE_VR_AGENTS_OBSERVER_H_
#define VSHL_CORE_INCLUDE_VR_AGENTS_OBSERVER_H_

#include <memory>

#include "core/include/VRRequestProcessorDelegate.h"
#include "interfaces/voiceagents/IVoiceAgentsChangeObserver.h"
#include "utilities/logging/Logger.h"

using namespace std;

namespace vshl {
namespace core {
/*
 * This class will observe the changes to the voiceagents data and transfers
 * the actual handling responsibility to its delegate.
 */
class VRAgentsObserver
    : public vshl::common::interfaces::IVoiceAgentsChangeObserver {
public:
  // Create a VRAgentsObserver.
  static shared_ptr<VRAgentsObserver>
  create(weak_ptr<VRRequestProcessorDelegate> delegate);

  ~VRAgentsObserver();

protected:
  void OnDefaultVoiceAgentChanged(
      shared_ptr<vshl::common::interfaces::IVoiceAgent> defaultVoiceAgent)
      override;
  void OnVoiceAgentAdded(
      shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) override;
  void OnVoiceAgentRemoved(
      shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) override;
  void OnVoiceAgentActiveWakeWordChanged(
      shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) override;
  void OnVoiceAgentActivated(
      shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) override;
  void OnVoiceAgentDeactivated(
      shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) override;

private:
  // Constructor
  VRAgentsObserver(weak_ptr<VRRequestProcessorDelegate> delegate);

  // Delegate that needs to be informed of the voiceagent data changes.
  weak_ptr<VRRequestProcessorDelegate> mWeakDelegate;
};

} // namespace core
} // namespace vshl

#endif // VSHL_CORE_INCLUDE_VR_AGENTS_OBSERVER_H_
