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
#include "appmanagement/AppController.h"

static string TAG = "vshl::appmanagement::AppController";

// Start an application
static string START_APP_API = "afm-main";
static string START_APP_VERB = "once";

// Display an application
static string DISPLAY_APP_API = "homescreen";
static string DISPLAY_APP_VERB = "tap_shortcut";

using Level = vshl::common::interfaces::ILogger::Level;

namespace vshl {
namespace appmanagement {

// Create appcontroller
unique_ptr<AppController> AppController::create(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) {
    auto appController = std::unique_ptr<AppController>(new AppController(logger, afbApi));
    return appController;
}

AppController::AppController(
    shared_ptr<vshl::common::interfaces::ILogger> logger,
    shared_ptr<vshl::common::interfaces::IAFBApi> afbApi) :
        mAfbApi(afbApi),
        mLogger(logger) {
}

bool AppController::startApp(const string& name, const string& version) {
    string appToStart = name + "@" + version;
    if (!mAfbApi) {
        mLogger->log(
            Level::ERROR, TAG, "Failed to start app: No AFB Api, app:" + appToStart + ", version: " + version);
        return false;
    }

    json_object* object = NULL;
    std::string error, info;
    int err = mAfbApi->callSync(START_APP_API, START_APP_VERB, json_object_new_string(appToStart.c_str()), &object, error, info);
    if (object) {
        free(object);
    }
    if (err) {
        mLogger->log(Level::ERROR, TAG, "Failed to start app:" + appToStart);
        return false;
    }

    mLogger->log(Level::INFO, TAG, "Starting app: " + appToStart);
    return true;
}

bool AppController::displayApp(const string& name, const string& version) {
	string appToDisplay = name + "@" + version;
    if (!mAfbApi) {
        mLogger->log(
            Level::ERROR, TAG, "Failed to display app: No AFB Api, app:" + name + ", version: " + version);
        return false;
    }

    json_object* object = NULL;
    std::string error, info;
    int err = mAfbApi->callSync(DISPLAY_APP_API, DISPLAY_APP_VERB, json_object_new_string(appToDisplay.c_str()), &object, error, info);
    if (object) {
        free(object);
    }
    if (err) {
        mLogger->log(Level::ERROR, TAG, "Failed to display app:" + appToDisplay);
        return false;
    }

    mLogger->log(Level::INFO, TAG, "Bringing app: " + appToDisplay + " to foreground.");
    return true;
}

}  // namespace appmanagement
}  // namespace vshl