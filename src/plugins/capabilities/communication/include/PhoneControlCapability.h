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

#ifndef VSHL_CAPABILITIES_PHONECONTROL_CAPABILITY_H_
#define VSHL_CAPABILITIES_PHONECONTROL_CAPABILITY_H_

#include <memory>

#include "interfaces/appmanagement/IAppController.h"
#include "interfaces/capabilities/ICapability.h"
#include "interfaces/utilities/logging/ILogger.h"

namespace vshl {
namespace capabilities {
namespace phonecontrol {

/*
 * PhoneControl capability. Calls are initiated in the endpoint.
 */
class PhoneControl : public common::interfaces::ICapability {
public:
    // Create a PhoneControl.
    static std::shared_ptr<PhoneControl> create(
        shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
        shared_ptr<vshl::common::interfaces::ILogger> logger);

    ~PhoneControl() = default;

protected:
    string getName() const override;

    list<string> getUpstreamMessages() const override;

    list<string> getDownstreamMessages() const override;

    void onMessagePublished(const string action) override;

private:
    PhoneControl(
        shared_ptr<vshl::interfaces::appmanagement::IAppController> appController,
        shared_ptr<vshl::common::interfaces::ILogger> logger);

    shared_ptr<vshl::interfaces::appmanagement::IAppController> mAppController;

    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

}  // namespace phonecontrol
}  // namespace capabilities
}  // namespace vshl

#endif  // VSHL_CAPABILITIES_PHONECONTROL_CAPABILITY_H_
