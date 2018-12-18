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
#include "capabilities/communication/include/PhoneControlCapability.h"
#include "capabilities/communication/include/PhoneControlMessages.h"

const string TAG = "vshl::capabilities::phonecontrol";
const string APP_NAME = "phone";
const string VERSION_NAME = "0.1";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace capabilities {
namespace phonecontrol {

// Create a phonecontrol.
shared_ptr<PhoneControl> PhoneControl::create(
    shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    auto phonecontrol = std::shared_ptr<PhoneControl>(new PhoneControl(appController, logger));
    return phonecontrol;
}

PhoneControl::PhoneControl(
    shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    mAppController = appController;
    mLogger = logger;
}

string PhoneControl::getName() const {
    return NAME;
}

list<string> PhoneControl::getUpstreamMessages() const {
    return PHONECONTROL_UPSTREAM_ACTIONS;
}

list<string> PhoneControl::getDownstreamMessages() const {
    return PHONECONTROL_DOWNSTREAM_ACTIONS;
}

void PhoneControl::onMessagePublished(const string action) {
    if (action == PHONECONTROL_DIAL) {
        mLogger->log(Level::INFO, TAG, "PhoneControl::onMessagePublished, Launcing Dialer app.");
        if (mAppController != nullptr) {
            mAppController->startApp(APP_NAME, VERSION_NAME);
            mAppController->displayApp(APP_NAME, VERSION_NAME);
        }
    }
}

}  // namespace phonecontrol
}  // namespace capabilities
}  // namespace vshl
