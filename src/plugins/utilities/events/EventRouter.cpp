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
#include "utilities/events/EventRouter.h"

static string TAG = "vshl::utilities::events::EventRouter";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace utilities {
namespace events {

unique_ptr<EventRouter> EventRouter::create(shared_ptr<vshl::common::interfaces::ILogger> logger) {
    return std::unique_ptr<EventRouter>(new EventRouter(logger));
}

EventRouter::EventRouter(shared_ptr<vshl::common::interfaces::ILogger> logger) : mLogger(logger) {
}

EventRouter::~EventRouter() {
    mEventFilters.clear();
}

bool EventRouter::handleIncomingEvent(const string eventName, const string voiceAgentId, const string payload) {
    for (auto eventFilter : mEventFilters) {
        if (eventFilter->onIncomingEvent(eventName, voiceAgentId, payload)) {
            return true;
        }
    }

    return false;
}

bool EventRouter::addEventFilter(shared_ptr<vshl::common::interfaces::IEventFilter> filter) {
    if (!filter) {
        mLogger->log(Level::ERROR, TAG, "Failed to add event filter. Invalid arguments.");
        return false;
    }

    mEventFilters.insert(filter);
    return true;
}

bool EventRouter::removeEventFilter(shared_ptr<vshl::common::interfaces::IEventFilter> filter) {
    if (!filter) {
        mLogger->log(Level::ERROR, TAG, "Failed to add remove filter. Invalid arguments.");
        return false;
    }

    mEventFilters.erase(filter);
    return true;
}

}  // namespace events
}  // namespace utilities
}  // namespace vshl
