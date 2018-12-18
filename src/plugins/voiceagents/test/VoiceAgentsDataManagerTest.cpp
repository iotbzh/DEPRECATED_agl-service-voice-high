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

#include "voiceagents/VoiceAgentsDataManager.h"

#include "test/common/ConsoleLogger.h"
#include "test/mocks/AFBApiMock.h"
#include "test/mocks/VoiceAgentsChangeObserverMock.h"
#include "voiceagents/test/VoiceAgentsTestData.h"

using namespace vshl::common::interfaces;
using namespace vshl::voiceagents;

using namespace vshl::test::common;

namespace vshl {
namespace test {

class VoiceAgentDataManagerTest : public ::testing::Test {
protected:
  void SetUp() override {
    mConsoleLogger = std::make_shared<ConsoleLogger>();
    mAfbApi = std::make_shared<::testing::NiceMock<AFBApiMock>>();
    mVADataManager = VoiceAgentsDataManager::create(mConsoleLogger, mAfbApi);

    mAgentsChangeObserver = std::make_shared<
        ::testing::StrictMock<VoiceAgentsChangeObserverMock>>();
    mVADataManager->addVoiceAgentsChangeObserver(mAgentsChangeObserver);

    mVoiceAgentsData = getVoiceAgentsTestData();
  }

  void TearDown() override {
    mVADataManager->removeVoiceAgentsChangeObserver(mAgentsChangeObserver);
  }

  static bool addVoiceAgent(VoiceAgentsDataManager &mgr,
                            VoiceAgentTestData &data) {
    return mgr.addNewVoiceAgent(data.id, data.name, data.description, data.api,
                                data.vendor, data.activeWakeword, data.isActive,
                                data.wakewords);
  }

  static bool isEqual(const VoiceAgentTestData &lhs, const IVoiceAgent &rhs) {
    return lhs.id == rhs.getId() && lhs.name == rhs.getName() &&
           lhs.description == rhs.getDescription() && lhs.api == rhs.getApi() &&
           lhs.vendor == rhs.getVendor() &&
           lhs.activeWakeword == rhs.getActiveWakeword() &&
           lhs.isActive == rhs.isActive() &&
           *lhs.wakewords == *rhs.getWakeWords();
  }

  static std::shared_ptr<IVoiceAgent>
  findVoiceAgent(std::set<std::shared_ptr<IVoiceAgent>> &voiceAgents,
                 std::string &vaId) {
    for (auto va : voiceAgents) {
      if (va->getId() == vaId)
        return va;
    }

    return nullptr;
  }

  std::shared_ptr<ConsoleLogger> mConsoleLogger;

  // It is a NiceMock because we don't want gtest to produce warnings about non
  // interesting calls.
  // The non interesting calls like createEvent is internal implementation
  // detail for many of the
  // tests in this class, and hence suppression of these warnings with NiceMock.
  std::shared_ptr<::testing::NiceMock<AFBApiMock>> mAfbApi;

  // It is a StrictMock because we want to fail the test for all non interesting
  // calls.
  std::shared_ptr<::testing::StrictMock<VoiceAgentsChangeObserverMock>>
      mAgentsChangeObserver;

  std::vector<VoiceAgentTestData> mVoiceAgentsData;
  std::unique_ptr<VoiceAgentsDataManager> mVADataManager;
};

TEST_F(VoiceAgentDataManagerTest, InitializesCorrectly) {
  ASSERT_NE(mVADataManager, nullptr);
  ASSERT_EQ(mVADataManager->getAllVoiceAgents().size(), 0);
  ASSERT_EQ(mVADataManager->getDefaultVoiceAgent(), std::string());
}

TEST_F(VoiceAgentDataManagerTest, addingVoiceAgentWithSameIdFails) {
  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(1);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_FALSE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));

  auto allVoiceAgents = mVADataManager->getAllVoiceAgents();
  ASSERT_EQ(allVoiceAgents.size(), 1);
}

TEST_F(VoiceAgentDataManagerTest,
       addingVoiceAgentWithNonExistentActiveWakewordFails) {
  auto voiceAgetData = mVoiceAgentsData[0];
  voiceAgetData.activeWakeword = "non-existent";
  ASSERT_FALSE(addVoiceAgent(*mVADataManager, voiceAgetData));

  auto allVoiceAgents = mVADataManager->getAllVoiceAgents();
  ASSERT_EQ(allVoiceAgents.size(), 0);
}

TEST_F(VoiceAgentDataManagerTest, canAddNewVoiceAgents) {
  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(2);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));

  auto allVoiceAgents = mVADataManager->getAllVoiceAgents();
  ASSERT_EQ(allVoiceAgents.size(), 1);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[1]));

  allVoiceAgents = mVADataManager->getAllVoiceAgents();
  ASSERT_EQ(allVoiceAgents.size(), 2);

  for (auto va : allVoiceAgents) {
    bool voiceAgentFound = false;
    for (auto vaData : mVoiceAgentsData) {
      if (isEqual(vaData, *va)) {
        voiceAgentFound = true;
        break;
      }
    }
    ASSERT_TRUE(voiceAgentFound);
  }
}

TEST_F(VoiceAgentDataManagerTest, removingUnknonwVoiceAgentFails) {
  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(1);

  ASSERT_FALSE(mVADataManager->removeVoiceAgent("non-existent-vaid"));

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_FALSE(mVADataManager->removeVoiceAgent("non-existent-vaid"));
}

TEST_F(VoiceAgentDataManagerTest, canRemoveVoiceAgents) {
  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(2);
  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentRemoved(::testing::_))
      .Times(2);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[1]));

  auto allVoiceAgents = mVADataManager->getAllVoiceAgents();
  ASSERT_EQ(allVoiceAgents.size(), 2);

  ASSERT_TRUE(mVADataManager->removeVoiceAgent(mVoiceAgentsData[0].id));

  allVoiceAgents = mVADataManager->getAllVoiceAgents();
  ASSERT_EQ(allVoiceAgents.size(), 1);

  ASSERT_TRUE(mVADataManager->removeVoiceAgent(mVoiceAgentsData[1].id));

  allVoiceAgents = mVADataManager->getAllVoiceAgents();
  ASSERT_EQ(allVoiceAgents.size(), 0);
}

TEST_F(VoiceAgentDataManagerTest, activatingNonExistentVoiceAgentsFails) {
  uint32_t result = mVADataManager->activateVoiceAgents({"non", "existent"});
  ASSERT_EQ(result, 0);

  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(2);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[1]));

  result = mVADataManager->activateVoiceAgents({"non", "existent"});
  ASSERT_EQ(result, 0);
}

TEST_F(VoiceAgentDataManagerTest, deactivatingNonExistentVoiceAgentsFails) {
  uint32_t result = mVADataManager->deactivateVoiceAgents({"non", "existent"});
  ASSERT_EQ(result, 0);

  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(2);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[1]));

  result = mVADataManager->deactivateVoiceAgents({"non", "existent"});
  ASSERT_EQ(result, 0);
}

TEST_F(VoiceAgentDataManagerTest, canActivateDeactivateVoiceAgents) {
  {
    ::testing::InSequence dummy;

    EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_))
        .Times(2);
    EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentDeactivated(::testing::_))
        .Times(1);
    EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentActivated(::testing::_))
        .Times(1);
  }

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[1]));

  std::string vaId = mVoiceAgentsData[0].id;

  auto allVA = mVADataManager->getAllVoiceAgents();
  auto voiceAgent = findVoiceAgent(allVA, vaId);
  ASSERT_NE(voiceAgent, nullptr);
  ASSERT_TRUE(voiceAgent->isActive());

  uint32_t result =
      mVADataManager->deactivateVoiceAgents({"non-existent", vaId});
  ASSERT_EQ(result, 1);
  ASSERT_FALSE(voiceAgent->isActive());

  // Try de-activating already de-activated agent
  result = mVADataManager->deactivateVoiceAgents({vaId});
  ASSERT_EQ(result, 1);
  ASSERT_FALSE(voiceAgent->isActive());

  result = mVADataManager->activateVoiceAgents({"non-existent", vaId});
  ASSERT_EQ(result, 1);
  ASSERT_TRUE(voiceAgent->isActive());

  // Try activating already activated agent
  result = mVADataManager->activateVoiceAgents({vaId});
  ASSERT_EQ(result, 1);
  ASSERT_TRUE(voiceAgent->isActive());
}

TEST_F(VoiceAgentDataManagerTest,
       NoDefaultAgentIsReturnedWhenNoDefaultAgentIsSet) {
  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(2);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[1]));

  std::string defaultAgentId = mVADataManager->getDefaultVoiceAgent();
  ASSERT_EQ(defaultAgentId, "");
}

TEST_F(VoiceAgentDataManagerTest, DefaultAgentCanBeSet) {
  EXPECT_CALL(*mAgentsChangeObserver, OnVoiceAgentAdded(::testing::_)).Times(2);

  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[0]));
  ASSERT_TRUE(addVoiceAgent(*mVADataManager, mVoiceAgentsData[1]));

  auto allAgents = mVADataManager->getAllVoiceAgents();
  std::string vaId1 = mVoiceAgentsData[1].id;
  std::string vaId2 = mVoiceAgentsData[0].id;
  auto va1 = findVoiceAgent(allAgents, vaId1);
  auto va2 = findVoiceAgent(allAgents, vaId2);

  {
    ::testing::InSequence dummy;

    EXPECT_CALL(*mAgentsChangeObserver, OnDefaultVoiceAgentChanged(va1))
        .Times(1);
    EXPECT_CALL(*mAgentsChangeObserver, OnDefaultVoiceAgentChanged(va2))
        .Times(1);
  }

  ASSERT_TRUE(mVADataManager->setDefaultVoiceAgent(vaId1));
  ASSERT_EQ(mVADataManager->getDefaultVoiceAgent(), vaId1);

  ASSERT_TRUE(mVADataManager->setDefaultVoiceAgent(vaId2));
  ASSERT_EQ(mVADataManager->getDefaultVoiceAgent(), vaId2);

  ASSERT_FALSE(mVADataManager->setDefaultVoiceAgent("non-existent"));
  ASSERT_EQ(mVADataManager->getDefaultVoiceAgent(), vaId2);

  // Setting default agent to already default agent shouldn't result in extra
  // callback to OnDefaultVoiceAgentChanged
  ASSERT_TRUE(mVADataManager->setDefaultVoiceAgent(vaId2));
  ASSERT_EQ(mVADataManager->getDefaultVoiceAgent(), vaId2);
}

} // namespace test
} // namespace vshl