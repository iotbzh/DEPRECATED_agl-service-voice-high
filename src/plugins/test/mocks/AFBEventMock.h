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
#ifndef VSHL_TEST_MOCKS_AFBEVENTMOCK_H_
#define VSHL_TEST_MOCKS_AFBEVENTMOCK_H_

#include <gmock/gmock.h>

#include "interfaces/afb/IAFBApi.h"

namespace vshl {
namespace test {

class AFBEventMock : public vshl::common::interfaces::IAFBApi::IAFBEvent {
public:
    void setName(const std::string& name) {
        mName = name;
    }

    std::string getName() const override {
        return mName;
    }

    MOCK_METHOD0(isValid, bool());
    MOCK_METHOD1(publishEvent, int(struct json_object* payload));
    MOCK_METHOD1(subscribe, bool(vshl::common::interfaces::IAFBRequest& request));
    MOCK_METHOD1(unsubscribe, bool(vshl::common::interfaces::IAFBRequest& request));

private:
    std::string mName;
};

}  // namespace test
}  // namespace vshl

#endif  // VSHL_TEST_MOCKS_AFBEVENTMOCK_H_