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

#include <gtest/gtest.h>

#include "core/include/VRRequest.h"
#include "voiceagents/include/VoiceAgent.h"
#include "voiceagents/test/VoiceAgentsTestData.h"

#include "test/common/ConsoleLogger.h"
#include "test/mocks/AFBApiMock.h"

using namespace vshl::core;
using namespace vshl::voiceagents;
using namespace vshl::test::common;

namespace vshl {
namespace test {

class VRRequestTest : public ::testing::Test {
protected:
    void SetUp() override {
        mRequestId = "Req-0001";

        mConsoleLogger = std::make_shared<ConsoleLogger>();
        mAfbApi = std::make_shared<::testing::StrictMock<AFBApiMock>>();

        auto vaTestData = *(getVoiceAgentsTestData().begin());
        mVoiceAgent = VoiceAgent::create(
            mConsoleLogger,
            vaTestData.id,
            vaTestData.name,
            vaTestData.description,
            vaTestData.api,
            vaTestData.vendor,
            vaTestData.activeWakeword,
            vaTestData.isActive,
            vaTestData.wakewords);

        mVRRequest = VRRequest::create(mConsoleLogger, mAfbApi, mRequestId, mVoiceAgent);
    }

    std::string mRequestId;
    std::shared_ptr<::testing::StrictMock<AFBApiMock>> mAfbApi;
    std::shared_ptr<ConsoleLogger> mConsoleLogger;
    std::shared_ptr<VoiceAgent> mVoiceAgent;

    std::shared_ptr<VRRequest> mVRRequest;
};

TEST_F(VRRequestTest, initializesCorrectly) {
    ASSERT_NE(mVRRequest, nullptr);
}

TEST_F(VRRequestTest, failsCreationOnInvalidParams) {
    auto vrRequest = VRRequest::create(mConsoleLogger, nullptr, mRequestId, mVoiceAgent);
    ASSERT_EQ(vrRequest, nullptr);

    vrRequest = VRRequest::create(nullptr, mAfbApi, mRequestId, mVoiceAgent);
    ASSERT_EQ(vrRequest, nullptr);
}

TEST_F(VRRequestTest, startsListeningSuccessfully) {
    EXPECT_CALL(
        *mAfbApi,
        callSync(
            mVoiceAgent->getApi(),
            VRRequest::VA_VERB_STARTLISTENING,
            ::testing::_,
            ::testing::_,
            ::testing::_,
            ::testing::_))
        .Times(1);

    ASSERT_TRUE(mVRRequest->startListening());
}

TEST_F(VRRequestTest, cancelsSuccessfully) {
    EXPECT_CALL(
        *mAfbApi,
        callSync(
            mVoiceAgent->getApi(), VRRequest::VA_VERB_CANCEL, ::testing::_, ::testing::_, ::testing::_, ::testing::_))
        .Times(1);

    ASSERT_TRUE(mVRRequest->cancel());
}

}  // namespace test
}  // namespace vshl