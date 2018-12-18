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

#ifndef VSHL_COMMON_INTERFACES_ICAPABILITY_H_
#define VSHL_COMMON_INTERFACES_ICAPABILITY_H_

#include <list>
#include <string>

using namespace std;

namespace vshl {
namespace common {
namespace interfaces {

/*
 * This interface defines the structure for a specific voiceagent capability.
 */
class ICapability {
public:
  /*
   * Returns the capability's name.
   */
  virtual string getName() const = 0;

  /*
   * Returns the list of upstream messages.
   */
  virtual list<string> getUpstreamMessages() const = 0;

  /*
   * Returns the list of downstream messages
   */
  virtual list<string> getDownstreamMessages() const = 0;

  /**
   * Method to notify that this capability messages
   * going to be published.
   */
  virtual void onMessagePublished(const string action) = 0;

  /**
   * Virtual destructor to assure proper cleanup of derived types.
   */
  virtual ~ICapability() = default;
};

} // namespace interfaces
} // namespace common
} // namespace vshl

#endif // VSHL_COMMON_INTERFACES_ICAPABILITY_H_
