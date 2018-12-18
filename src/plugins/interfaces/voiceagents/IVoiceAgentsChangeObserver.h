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

#ifndef VSHL_COMMON_INTERFACES_IVOICEAGENTSCHANGEOBSERVER_H_
#define VSHL_COMMON_INTERFACES_IVOICEAGENTSCHANGEOBSERVER_H_

#include <memory>
#include <string>

#include "interfaces/voiceagents/IVoiceAgent.h"

using namespace std;

namespace vshl {
namespace common {
namespace interfaces {

/*
 * This interface is used to observe changes to the voiceagents datastore.
 * The voiceagents data store is contained in the voiceagents module.
 */
class IVoiceAgentsChangeObserver {
public:
  /**
   * This method notifies the observers that the default voiceagent selection
   * has been updated.
   */
  virtual void
  OnDefaultVoiceAgentChanged(shared_ptr<IVoiceAgent> defaultVoiceAgent) = 0;

  /**
   * This method notifies the observers that a new voiceagent has been added.
   */
  virtual void OnVoiceAgentAdded(shared_ptr<IVoiceAgent> voiceAgent) = 0;

  /**
   * This method notifies the observers that a voiceagent is removed.
   */
  virtual void OnVoiceAgentRemoved(shared_ptr<IVoiceAgent> voiceAgent) = 0;

  /**
   * This method notifies the observers that active wakeword for a voiceagent is
   * updated.
   */
  virtual void
  OnVoiceAgentActiveWakeWordChanged(shared_ptr<IVoiceAgent> voiceAgent) = 0;

  /**
   * This method notifies the observers that the voiceagent has been activated.
   */
  virtual void OnVoiceAgentActivated(shared_ptr<IVoiceAgent> voiceAgent) = 0;

  /**
   * This method notifies the observers that the voiceagent has been activated.
   */
  virtual void OnVoiceAgentDeactivated(shared_ptr<IVoiceAgent> voiceAgent) = 0;

  /**
   * Virtual destructor to assure proper cleanup of derived types.
   */
  virtual ~IVoiceAgentsChangeObserver() = default;
};

} // namespace interfaces
} // namespace common
} // namespace vshl

#endif // VSHL_COMMON_INTERFACES_IVOICEAGENTSCHANGEOBSERVER_H_
