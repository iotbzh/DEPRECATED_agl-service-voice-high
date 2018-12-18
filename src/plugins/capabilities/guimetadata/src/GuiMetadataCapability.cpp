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
#include "capabilities/guimetadata/include/GuiMetadataCapability.h"
#include "capabilities/guimetadata/include/GuiMetadataMessages.h"

const string TAG = "vshl::capabilities::guimetadata";
const string APP_NAME = "ics-alexa-app";
const string VERSION_NAME = "0.1";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace capabilities {
namespace guimetadata {

// Create a GuiMetadata.
shared_ptr<GuiMetadata> GuiMetadata::create(
    shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    auto guiMetadata = std::shared_ptr<GuiMetadata>(new GuiMetadata(appController, logger));
    return guiMetadata;
}

GuiMetadata::GuiMetadata(
    shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
    shared_ptr<vshl::common::interfaces::ILogger> logger) {
    mAppController = appController;
    mLogger = logger;
}

string GuiMetadata::getName() const {
    return NAME;
}

list<string> GuiMetadata::getUpstreamMessages() const {
    return GUIMETADATA_UPSTREAM_ACTIONS;
}

list<string> GuiMetadata::getDownstreamMessages() const {
    return GUIMETADATA_DOWNSTREAM_ACTIONS;
}

void GuiMetadata::onMessagePublished(const string action) {
    if (action == GUIMETADATA_RENDER_TEMPLATE) {
        mLogger->log(Level::INFO, TAG, "GuiMetadata::onMessagePublished, Launcing GUIMetada app.");
        if (mAppController != nullptr) {
            mAppController->startApp(APP_NAME, VERSION_NAME);
            mAppController->displayApp(APP_NAME, VERSION_NAME);
        }
    }
}

}  // namespace guimetadata
}  // namespace capabilities
}  // namespace vshl
