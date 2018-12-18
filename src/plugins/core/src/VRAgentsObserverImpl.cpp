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
#include "core/include/VRAgentsObserver.h"

namespace vshl {
namespace core {

shared_ptr<VRAgentsObserver> VRAgentsObserver::create(weak_ptr<VRRequestProcessorDelegate> delegate) {
    auto observer = std::shared_ptr<VRAgentsObserver>(new VRAgentsObserver(delegate));
    return observer;
}

VRAgentsObserver::VRAgentsObserver(weak_ptr<VRRequestProcessorDelegate> delegate) {
    mWeakDelegate = delegate;
}

VRAgentsObserver::~VRAgentsObserver() {
}

void VRAgentsObserver::OnDefaultVoiceAgentChanged(shared_ptr<vshl::common::interfaces::IVoiceAgent> defaultVoiceAgent) {
    if (auto delegate = mWeakDelegate.lock()) {
        delegate->setDefaultVoiceAgent(defaultVoiceAgent);
    }
}

void VRAgentsObserver::OnVoiceAgentAdded(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
}

void VRAgentsObserver::OnVoiceAgentRemoved(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
}

void VRAgentsObserver::OnVoiceAgentActiveWakeWordChanged(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
    // Not Implemented
}

void VRAgentsObserver::OnVoiceAgentActivated(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
}

void VRAgentsObserver::OnVoiceAgentDeactivated(shared_ptr<vshl::common::interfaces::IVoiceAgent> voiceAgent) {
}
}  // namespace core
}  // namespace vshl
