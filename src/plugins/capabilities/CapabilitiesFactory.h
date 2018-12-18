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
#ifndef VSHL_CAPABILITIES_CAPABILITIESFACTORY_H_
#define VSHL_CAPABILITIES_CAPABILITIESFACTORY_H_

#include <memory>

#include "interfaces/appmanagement/IAppController.h"
#include "interfaces/capabilities/ICapability.h"
#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace capabilities {
/*
 * Factory for creating different capability objects.
 */
class CapabilitiesFactory {
public:
    // Create CapabilitiesFactory
    static std::unique_ptr<CapabilitiesFactory> create(
        shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
        shared_ptr<vshl::common::interfaces::ILogger> logger);

    // GUI Metadata capability
    std::shared_ptr<common::interfaces::ICapability> getGuiMetadata();

    // Phone call control capability
    std::shared_ptr<common::interfaces::ICapability> getPhoneControl();

    // Navigation capability
    std::shared_ptr<common::interfaces::ICapability> getNavigation();

    // Destructor
    ~CapabilitiesFactory() = default;

private:
    // Constructor
    CapabilitiesFactory(
        shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
        shared_ptr<vshl::common::interfaces::ILogger> logger);

    // Capabilities
    shared_ptr<vshl::common::interfaces::ICapability> mGuiMetadata;
    shared_ptr<vshl::common::interfaces::ICapability> mPhoneControl;
    shared_ptr<vshl::common::interfaces::ICapability> mNavigation;

    // App Controller
    shared_ptr<vshl::interfaces::appmanagement::IAppController> mAppController;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace capabilities
}  // namespace vshl

#endif  // VSHL_CAPABILITIES_CAPABILITIESFACTORY_H_
