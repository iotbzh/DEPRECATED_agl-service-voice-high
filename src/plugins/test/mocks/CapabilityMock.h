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
#ifndef VSHL_TEST_MOCKS_CAPABILITYMOCK_H_
#define VSHL_TEST_MOCKS_CAPABILITYMOCK_H_

#include <gmock/gmock.h>

#include "interfaces/capabilities/ICapability.h"

namespace vshl {
namespace test {

class CapabilityMock : public vshl::common::interfaces::ICapability {
public:
    MOCK_CONST_METHOD0(getName, std::string());
    MOCK_CONST_METHOD0(getUpstreamMessages, std::list<std::string>());
    MOCK_CONST_METHOD0(getDownstreamMessages, std::list<std::string>());
};

}  // namespace test
}  // namespace vshl

#endif  // VSHL_TEST_MOCKS_CAPABILITYMOCK_H_