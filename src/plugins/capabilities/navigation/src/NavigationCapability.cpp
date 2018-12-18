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
#include "capabilities/navigation/include/NavigationCapability.h"
#include "capabilities/navigation/include/NavigationMessages.h"

const string TAG = "vshl::capabilities::navigation";
const string APP_NAME = "navigation";
const string VERSION_NAME = "0.1";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace capabilities {
namespace navigation {

// Create a Navigation.
shared_ptr<Navigation> Navigation::create(
	shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    auto navigation = std::shared_ptr<Navigation>(new Navigation(appController, logger));
    return navigation;
}

Navigation::Navigation(
    shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    mAppController = appController;
    mLogger = logger;
}

string Navigation::getName() const {
    return NAME;
}

list<string> Navigation::getUpstreamMessages() const {
    return NAVIGATION_UPSTREAM_ACTIONS;
}

list<string> Navigation::getDownstreamMessages() const {
    return NAVIGATION_DOWNSTREAM_ACTIONS;
}

void Navigation::onMessagePublished(const string action) {
    if (action == NAVIGATION_SET_DESTINATION) {
        mLogger->log(Level::INFO, TAG, "Navigation::onMessagePublished, Launcing Navigation app.");
        if (mAppController != nullptr) {
            mAppController->startApp(APP_NAME, VERSION_NAME);
            mAppController->displayApp(APP_NAME, VERSION_NAME);
        }
    }
}

}  // namespace navigation
}  // namespace capabilities
}  // namespace vshl
