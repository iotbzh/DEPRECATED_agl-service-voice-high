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
#ifndef VSHL_UTILITIES_EVENTS_EVENTMANAGER_H_
#define VSHL_UTILITIES_EVENTS_EVENTMANAGER_H_

#include <memory>
#include <string>
#include <unordered_set>

#include "interfaces/utilities/events/IEventFilter.h"
#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace utilities {
namespace events {
/*
 * This class is responsible for routing incoming events to
 * the appropriate event listener for consumption.
 * Note: The listeners should implement the IEventFilter class.
 */
class EventRouter {
public:
    static unique_ptr<EventRouter> create(shared_ptr<vshl::common::interfaces::ILogger> logger);

    // Destructor
    ~EventRouter();

    // Add event filter as listerner.
    bool addEventFilter(shared_ptr<vshl::common::interfaces::IEventFilter> filter);

    // Remove event filter as listerner.
    bool removeEventFilter(shared_ptr<vshl::common::interfaces::IEventFilter> filter);

    // This method is called by the controller for routing
    // the event to appropriate listener.
    bool handleIncomingEvent(const string eventName, const string voiceAgentId, const string payload);

private:
    EventRouter(shared_ptr<vshl::common::interfaces::ILogger> logger);

    // set of event filters.
    unordered_set<shared_ptr<vshl::common::interfaces::IEventFilter>> mEventFilters;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace events
}  // namespace utilities
}  // namespace vshl

#endif  // VSHL_UTILITIES_EVENTS_EVENTROUTER_H_
