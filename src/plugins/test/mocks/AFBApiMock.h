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
#ifndef VSHL_TEST_MOCKS_AFBAPIMOCK_H_
#define VSHL_TEST_MOCKS_AFBAPIMOCK_H_

#include <gmock/gmock.h>

#include "interfaces/afb/IAFBApi.h"

namespace vshl {
namespace test {

class AFBApiMock : public vshl::common::interfaces::IAFBApi {
public:
    MOCK_METHOD1(createEvent, std::shared_ptr<IAFBEvent>(const std::string& eventName));
    MOCK_METHOD6(
        callSync,
        int(const std::string& api,
            const std::string& verb,
            struct json_object* request,
            struct json_object** result,
            std::string& error,
            std::string& info));
};

}  // namespace test
}  // namespace vshl

#endif  // VSHL_TEST_MOCKS_AFBAPIMOCK_H_