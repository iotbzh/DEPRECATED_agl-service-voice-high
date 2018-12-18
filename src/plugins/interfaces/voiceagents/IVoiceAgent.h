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

#ifndef VSHL_COMMON_INTERFACES_IVOICEAGENT_H_
#define VSHL_COMMON_INTERFACES_IVOICEAGENT_H_

#include <memory>
#include <string>
#include <unordered_set>

using namespace std;

namespace vshl {
namespace common {
namespace interfaces {

/*
 * This interface define the structure for VoiceAgent Information.
 * The implementation of this structure is owned by voiceagents module.
 */
class IVoiceAgent {
public:
  /*
   * Set the active wakeword for this voiceagent
   */
  virtual bool setActiveWakeWord(const string &wakeword) = 0;

  /*
   * Sets the activation state of this voiceagent
   */
  virtual void setIsActive(bool active) = 0;

  /*
   * Returns the voiceagent's ID.
   */
  virtual string getId() const = 0;

  /*
   * Returns the voiceagent's name.
   */
  virtual string getName() const = 0;

  /*
   * Returns the voiceagent's description.
   */
  virtual string getDescription() const = 0;

  /*
   * Returns the voiceagent's API.
   */
  virtual string getApi() const = 0;

  /*
   * Returns the voiceagent's vendor information/
   */
  virtual string getVendor() const = 0;

  /*
   * Returns the list of wakewords mapped to the voiceagent.
   */
  virtual shared_ptr<unordered_set<string>> getWakeWords() const = 0;

  /*
   * Returns true if the voiceagent is active. False otherwise.
   */
  virtual bool isActive() const = 0;

  /*
   * Returns the active wakeword for the voiceagent.
   */
  virtual string getActiveWakeword() const = 0;

  /**
   * Virtual destructor to assure proper cleanup of derived types.
   */
  virtual ~IVoiceAgent() = default;
};

} // namespace interfaces
} // namespace common
} // namespace vshl

#endif // VSHL_COMMON_INTERFACES_IVOICEAGENT_H_
