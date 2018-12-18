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
#ifndef VSHL_CAPABILITIES_GUIMETADATA_ACTIONS_H_
#define VSHL_CAPABILITIES_GUIMETADATA_ACTIONS_H_

#include <list>
#include <string>

using namespace std;

namespace vshl {
namespace capabilities {
namespace guimetadata {

static string NAME = "guimetadata";

// Supported actions from VA -> Apps
static string GUIMETADATA_RENDER_TEMPLATE = "render_template";
static string GUIMETADATA_CLEAR_TEMPLATE = "clear_template";
static string GUIMETADATA_RENDER_PLAYER_INFO = "render_player_info";
static string GUIMETADATA_CLEAR_PLAYER_INFO = "clear_player_info";

// Supported actions from Apps -> VA

// List of actions that are delivered from VA -> Apps
static list<string> GUIMETADATA_UPSTREAM_ACTIONS = {GUIMETADATA_RENDER_TEMPLATE,
                                                    GUIMETADATA_CLEAR_TEMPLATE,
                                                    GUIMETADATA_RENDER_PLAYER_INFO,
                                                    GUIMETADATA_CLEAR_PLAYER_INFO};

// List of actions that are delivered from Apps -> VA
static list<string> GUIMETADATA_DOWNSTREAM_ACTIONS = {};

}  // namespace guimetadata
}  // namespace capabilities
}  // namespace vshl

#endif  // VSHL_CAPABILITIES_GUIMETADATA_ACTIONS_H_
