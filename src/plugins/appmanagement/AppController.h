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

#ifndef VSHL_APPMANAGEMENT_APPCONTROLLER_H_
#define VSHL_APPMANAGEMENT_APPCONTROLLER_H_

#include <memory>

#include "interfaces/afb/IAFBApi.h"
#include "interfaces/appmanagement/IAppController.h"
#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace appmanagement {

/*
 * IAppController interface implementation.
 */
class AppController: public vshl::interfaces::appmanagement::IAppController  {
public:
    // Create a AppController.
    static unique_ptr<AppController> create(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    ~AppController() = default;

protected:

    bool startApp(const string& name, const string& version) override;

    bool displayApp(const string& name, const string& version) override;

private:
    AppController(
        shared_ptr<vshl::common::interfaces::ILogger> logger,
        shared_ptr<vshl::common::interfaces::IAFBApi> afbApi);

    // Binding API reference
    shared_ptr<vshl::common::interfaces::IAFBApi> mAfbApi;

    // Logger
    shared_ptr<vshl::common::interfaces::ILogger> mLogger;
};

} // namespace appmanagement
} // namespace vshl

#endif // VSHL_APPMANAGEMENT_APPCONTROLLER_H_