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
#ifndef VSHL_TEST_MOCKS_AFBREQUESTMOCK_H_
#define VSHL_TEST_MOCKS_AFBREQUESTMOCK_H_

#include <gmock/gmock.h>

#include "interfaces/afb/IAFBApi.h"

namespace vshl {
namespace test {

class AFBRequestMock : public vshl::common::interfaces::IAFBRequest {
public:
    MOCK_METHOD0(getNativeRequest, void*());
};

}  // namespace test
}  // namespace vshl

#endif  // VSHL_TEST_MOCKS_AFBREQUESTMOCK_H_