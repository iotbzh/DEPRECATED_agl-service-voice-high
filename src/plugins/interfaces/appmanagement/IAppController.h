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

#ifndef VSHL_INTERFACES_APPMANAGEMENT_IAPPCONTROLLER_H_
#define VSHL_INTERFACES_APPMANAGEMENT_IAPPCONTROLLER_H_

using namespace std;

namespace vshl {
namespace interfaces {
namespace appmanagement {
/*
 * This interface is used to control the application lifecycle.
 * Has implementation for starting and displaying an application.
 */
class IAppController {
public:
  /**
   * start the application.
   */
  virtual bool startApp(const string& name, const string& version) = 0;

  /**
   * display the application.
   */
  virtual bool displayApp(const string& name, const string& version) = 0;

  /**
   * Virtual destructor to assure proper cleanup of derived types.
   */
  virtual ~IAppController() = default;
};

} // namespace appmanagement
} // namespace interfaces
} // namespace vshl

#endif // VSHL_INTERFACES_APPMANAGEMENT_IAPPCONTROLLER_H_