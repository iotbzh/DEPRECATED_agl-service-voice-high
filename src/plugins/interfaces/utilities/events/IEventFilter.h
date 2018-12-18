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
#ifndef VSHL_COMMON_INTERFACES_IEVENTFILTER_H_
#define VSHL_COMMON_INTERFACES_IEVENTFILTER_H_

#include <string>

using namespace std;

namespace vshl {
namespace common {
namespace interfaces {
/*
 * This is an abstract class that is responsible for filtering the events
 * that are delivered to the high level voice service from apps or voiceagents.
 */
class IEventFilter {
public:
    // Name of the event filter.
    virtual string getName() = 0;

    // Every event filter needs to implement this method and
    // return true if consuming the event or false otherwise.
    virtual bool onIncomingEvent(const string eventName, const string voiceAgentId, const string payload) = 0;

    // Destructor
    virtual ~IEventFilter() = default;
};

}  // namespace interfaces
}  // namespace common
}  // namespace vshl

#endif  // VSHL_COMMON_INTERFACES_IEVENTFILTER_H_
