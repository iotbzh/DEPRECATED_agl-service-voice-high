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

#include "capabilities/CapabilitiesFactory.h"

#include "capabilities/communication/include/PhoneControlCapability.h"
#include "capabilities/guimetadata/include/GuiMetadataCapability.h"
#include "capabilities/navigation/include/NavigationCapability.h"

static string TAG = "vshl::core::CapabilitiesFactory";

namespace vshl {
namespace capabilities {

// Create CapabilitiesFactory
std::unique_ptr<CapabilitiesFactory> CapabilitiesFactory::create(
    shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    auto capabilitiesFactory = std::unique_ptr<CapabilitiesFactory>(new CapabilitiesFactory(appController, logger));
    return capabilitiesFactory;
}

CapabilitiesFactory::CapabilitiesFactory(
    shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    mLogger = logger;
    mAppController = appController;
}

std::shared_ptr<common::interfaces::ICapability> CapabilitiesFactory::getGuiMetadata() {
    if (!mGuiMetadata) {
        mGuiMetadata = vshl::capabilities::guimetadata::GuiMetadata::create(mAppController, mLogger);
    }
    return mGuiMetadata;
}

std::shared_ptr<common::interfaces::ICapability> CapabilitiesFactory::getPhoneControl() {
    if (!mPhoneControl) {
        mPhoneControl = vshl::capabilities::phonecontrol::PhoneControl::create(mAppController, mLogger);
    }
    return mPhoneControl;
}

std::shared_ptr<common::interfaces::ICapability> CapabilitiesFactory::getNavigation(
    ) {
    if (!mNavigation) {
        mNavigation = vshl::capabilities::navigation::Navigation::create(mAppController, mLogger);
    }
    return mNavigation;
}

}  // namespace capabilities
}  // namespace vshl