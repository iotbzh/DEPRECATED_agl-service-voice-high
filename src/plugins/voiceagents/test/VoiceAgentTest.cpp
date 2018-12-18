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

#include "voiceagents/include/VoiceAgent.h"

#include "voiceagents/test/VoiceAgentsTestData.h"
#include "test/common/ConsoleLogger.h"

using namespace vshl::voiceagents;
using namespace vshl::test::common;

namespace vshl {
namespace test {

class VoiceAgentTest : public ::testing::Test {
protected:
    void SetUp() override {
        mConsoleLogger = std::make_shared<ConsoleLogger>();

        mVoiceAgentData = *(getVoiceAgentsTestData().begin());
        mVoiceAgent = VoiceAgent::create(
            mConsoleLogger,
            mVoiceAgentData.id,
            mVoiceAgentData.name,
            mVoiceAgentData.description,
            mVoiceAgentData.api,
            mVoiceAgentData.vendor,
            mVoiceAgentData.activeWakeword,
            mVoiceAgentData.isActive,
            mVoiceAgentData.wakewords);
    }

    std::shared_ptr<ConsoleLogger> mConsoleLogger;
    std::shared_ptr<VoiceAgent> mVoiceAgent;
    VoiceAgentTestData mVoiceAgentData;
};

TEST_F(VoiceAgentTest, InitializesCorrectly) {
    ASSERT_NE(mVoiceAgent, nullptr);
    ASSERT_EQ(mVoiceAgent->getId(), mVoiceAgentData.id);
    ASSERT_EQ(mVoiceAgent->getName(), mVoiceAgentData.name);
    ASSERT_EQ(mVoiceAgent->getDescription(), mVoiceAgentData.description);
    ASSERT_EQ(mVoiceAgent->getApi(), mVoiceAgentData.api);
    ASSERT_EQ(mVoiceAgent->getVendor(), mVoiceAgentData.vendor);
    ASSERT_EQ(mVoiceAgent->getActiveWakeword(), mVoiceAgentData.activeWakeword);
    ASSERT_EQ(mVoiceAgent->isActive(), mVoiceAgentData.isActive);

    std::unordered_set<std::string> wakeWords = *mVoiceAgentData.wakewords;
    ASSERT_EQ(*(mVoiceAgent->getWakeWords()), wakeWords);
}

TEST_F(VoiceAgentTest, FailsCreationOnNonExistentWakeword) {
    std::string nonExistentWW = "non-existent";
    auto voiceAgent = VoiceAgent::create(
        mConsoleLogger,
        mVoiceAgentData.id,
        mVoiceAgentData.name,
        mVoiceAgentData.description,
        mVoiceAgentData.api,
        mVoiceAgentData.vendor,
        nonExistentWW,
        mVoiceAgentData.isActive,
        mVoiceAgentData.wakewords);
    ASSERT_EQ(voiceAgent, nullptr);
}

TEST_F(VoiceAgentTest, SetsWakewordCorrectly) {
    std::string wakeword = *(mVoiceAgentData.wakewords->begin());
    ASSERT_TRUE(mVoiceAgent->setActiveWakeWord(wakeword));
    ASSERT_EQ(mVoiceAgent->getActiveWakeword(), wakeword);
}

TEST_F(VoiceAgentTest, FailsToSetNonExistentWakeword) {
    std::string nonExistentWW = "non-existent";
    ASSERT_FALSE(mVoiceAgent->setActiveWakeWord(nonExistentWW));
    ASSERT_EQ(mVoiceAgent->getActiveWakeword(), mVoiceAgentData.activeWakeword);
}

}  // namespace test
}  // namespace vshl