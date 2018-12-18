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
#ifndef VSHL_CAPABILITIES_NAVIGATION_ACTIONS_H_
#define VSHL_CAPABILITIES_NAVIGATION_ACTIONS_H_

#include <list>
#include <string>

using namespace std;

namespace vshl {
namespace capabilities {
namespace navigation {

static string NAME = "navigation";

// Supported actions from VA -> Apps
static string NAVIGATION_SET_DESTINATION = "set_destination";
static string NAVIGATION_CANCEL = "cancel_navigation";

// Supported actions from Apps -> VA

// List of actions that are delivered from VA -> Apps
static list<string> NAVIGATION_UPSTREAM_ACTIONS = {
    NAVIGATION_SET_DESTINATION,
    NAVIGATION_CANCEL,
};

// List of actions that are delivered from Apps -> VA
static list<string> NAVIGATION_DOWNSTREAM_ACTIONS = {};

}  // namespace navigation
}  // namespace capabilities
}  // namespace vshl

#endif  // VSHL_CAPABILITIES_NAVIGATION_ACTIONS_H_
