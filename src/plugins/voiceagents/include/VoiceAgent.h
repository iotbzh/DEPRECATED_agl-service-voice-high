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
#ifndef VSHL_VOICEAGENTS_INCLUDE_VOICEAGENT_H_
#define VSHL_VOICEAGENTS_INCLUDE_VOICEAGENT_H_

#include <memory>
#include <unordered_set>

#include "interfaces/utilities/logging/ILogger.h"
#include "interfaces/voiceagents/IVoiceAgent.h"

using namespace std;

namespace vshl {
namespace voiceagents {
/*
 * Default implementation of IVoiceAgent interface.
 */
class VoiceAgent : public vshl::common::interfaces::IVoiceAgent {
public:
  // Creates @c VoiceAgent instance
  static shared_ptr<VoiceAgent>
  create(shared_ptr<vshl::common::interfaces::ILogger> logger, const string &id,
         const string &name, const string &description, const string &api,
         const string &vendor, const string &activeWakeword,
         const bool isActive,
         const shared_ptr<unordered_set<string>> wakewords);

  // Destructor
  ~VoiceAgent();

  // IVoiceAgent overriden methods
  bool setActiveWakeWord(const string &wakeword) override;
  void setIsActive(bool active) override;
  string getId() const override;
  string getName() const override;
  string getDescription() const override;
  string getApi() const override;
  string getVendor() const override;
  shared_ptr<unordered_set<string>> getWakeWords() const override;
  bool isActive() const override;
  string getActiveWakeword() const override;

private:
  // Constructor
  VoiceAgent(shared_ptr<vshl::common::interfaces::ILogger> logger,
             const string &id, const string &name, const string &description,
             const string &api, const string &vendor,
             const string &activeWakeword, const bool isActive,
             const shared_ptr<unordered_set<string>> wakewords);

  // Logger
  shared_ptr<vshl::common::interfaces::ILogger> mLogger;

  // Id
  string mId;

  // Name
  string mName;

  // Description
  string mDescription;

  // API
  string mApi;

  // Vendor
  string mVendor;

  // Active wakeword
  string mActiveWakeword;

  // Active ??
  bool mIsActive;

  // Wakewords
  shared_ptr<unordered_set<string>> mWakewords;
};

} // namespace voiceagents
} // namespace vshl

#endif // VSHL_VOICEAGENTS_INCLUDE_VOICEAGENT_H_
