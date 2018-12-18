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

#include "capabilities/core/include/SubscriberForwarder.h"

#include "test/common/ConsoleLogger.h"
#include "test/mocks/AFBApiMock.h"
#include "test/mocks/AFBEventMock.h"
#include "test/mocks/AFBRequestMock.h"
#include "test/mocks/CapabilityMock.h"

using namespace vshl::common::interfaces;
using namespace vshl::capabilities::core;
using namespace vshl::test::common;

namespace vshl {
namespace test {

class SubscriberForwarderTest : public ::testing::Test {
protected:
    void SetUp() override {
        mConsoleLogger = std::make_shared<ConsoleLogger>();
        mAfbApi = std::make_shared<::testing::StrictMock<AFBApiMock>>();

        mEventCreatorFn = [](const std::string& eventName) -> std::shared_ptr<IAFBApi::IAFBEvent> {
            std::shared_ptr<AFBEventMock> mockEvent(new ::testing::StrictMock<AFBEventMock>());
            mockEvent->setName(eventName);
            return mockEvent;
        };
    }

    std::shared_ptr<SubscriberForwarder> createSubscriberForwarder(
        std::shared_ptr<::testing::StrictMock<CapabilityMock>> capability) {
        EXPECT_CALL(*capability, getUpstreamMessages()).Times(1);
        EXPECT_CALL(*capability, getDownstreamMessages()).Times(1);

        return SubscriberForwarder::create(mConsoleLogger, mAfbApi, capability);
    }

    std::shared_ptr<::testing::StrictMock<AFBApiMock>> mAfbApi;
    std::shared_ptr<ConsoleLogger> mConsoleLogger;
    std::function<std::shared_ptr<IAFBApi::IAFBEvent>(const std::string&)> mEventCreatorFn;
    std::shared_ptr<::testing::StrictMock<AFBEventMock>> mAfbEventMock;
};

TEST_F(SubscriberForwarderTest, failsInitializationOnInvalidParams) {
    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();

    auto forwarder = SubscriberForwarder::create(mConsoleLogger, mAfbApi, nullptr);
    ASSERT_EQ(forwarder, nullptr);

    forwarder = SubscriberForwarder::create(mConsoleLogger, nullptr, capability);
    ASSERT_EQ(forwarder, nullptr);

    forwarder = SubscriberForwarder::create(nullptr, mAfbApi, capability);
    ASSERT_EQ(forwarder, nullptr);
}

TEST_F(SubscriberForwarderTest, initializesCorrectly) {
    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    auto forwarder = createSubscriberForwarder(capability);

    ASSERT_NE(forwarder, nullptr);
}

TEST_F(SubscriberForwarderTest, createsEventsOnInitialization) {
    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Invoke(mEventCreatorFn));

    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto itCapability = upstreamEvents.begin();
    EXPECT_CALL(*mAfbApi, createEvent(*itCapability)).Times(1);
    itCapability++;
    EXPECT_CALL(*mAfbApi, createEvent(*itCapability)).Times(1);

    itCapability = downstreamEvents.begin();
    EXPECT_CALL(*mAfbApi, createEvent(*itCapability)).Times(1);
    itCapability++;
    EXPECT_CALL(*mAfbApi, createEvent(*itCapability)).Times(1);

    auto forwarder = createSubscriberForwarder(capability);
    ASSERT_NE(forwarder, nullptr);
}

TEST_F(SubscriberForwarderTest, canNotSubscribeToNonExistentEvents) {
    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Invoke(mEventCreatorFn));
    EXPECT_CALL(*mAfbApi, createEvent(::testing::_)).Times(4);

    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto forwarder = createSubscriberForwarder(capability);
    ASSERT_NE(forwarder, nullptr);

    auto nonExistentEvents = std::list<std::string>({"non", "existent", "events"});
    auto itCapability = nonExistentEvents.begin();
    auto request = std::make_shared<::testing::StrictMock<AFBRequestMock>>();
    ASSERT_FALSE(forwarder->subscribe(*request, *itCapability++));
    ASSERT_FALSE(forwarder->subscribe(*request, *itCapability++));
    ASSERT_FALSE(forwarder->subscribe(*request, *itCapability++));
}

TEST_F(SubscriberForwarderTest, canNotSubscribeOnEventCreationFailure) {
    // Fail the event creation and return null event.
    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Return(nullptr));
    EXPECT_CALL(*mAfbApi, createEvent(::testing::_)).Times(4);

    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto forwarder = createSubscriberForwarder(capability);
    ASSERT_NE(forwarder, nullptr);

    auto itCapability = downstreamEvents.begin();
    auto request = std::make_shared<::testing::StrictMock<AFBRequestMock>>();
    std::string payload = "The answer to life the universe and everything = 42";
    ASSERT_FALSE(forwarder->subscribe(*request, *itCapability++));
    ASSERT_FALSE(forwarder->subscribe(*request, *itCapability++));
    itCapability = upstreamEvents.begin();
    ASSERT_FALSE(forwarder->subscribe(*request, *itCapability++));
    ASSERT_FALSE(forwarder->subscribe(*request, *itCapability++));
}

TEST_F(SubscriberForwarderTest, canSubscribeToEvents) {
    std::shared_ptr<AFBEventMock> mockEvent(new ::testing::StrictMock<AFBEventMock>());
    ON_CALL(*mockEvent, subscribe(::testing::_)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(*mockEvent, subscribe(::testing::_)).Times(4);
    auto eventCreator = [mockEvent](const std::string& eventName) -> std::shared_ptr<IAFBApi::IAFBEvent> {
        return mockEvent;
    };

    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Invoke(eventCreator));
    EXPECT_CALL(*mAfbApi, createEvent(::testing::_)).Times(4);

    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto forwarder = createSubscriberForwarder(capability);
    ASSERT_NE(forwarder, nullptr);

    auto itCapability = downstreamEvents.begin();
    auto request = std::make_shared<::testing::StrictMock<AFBRequestMock>>();
    ASSERT_TRUE(forwarder->subscribe(*request, *itCapability++));
    ASSERT_TRUE(forwarder->subscribe(*request, *itCapability));
    itCapability = upstreamEvents.begin();
    ASSERT_TRUE(forwarder->subscribe(*request, *itCapability++));
    ASSERT_TRUE(forwarder->subscribe(*request, *itCapability));
}

TEST_F(SubscriberForwarderTest, canNotPublishNonExistentEvents) {
    std::shared_ptr<AFBEventMock> mockEvent(new ::testing::StrictMock<AFBEventMock>());
    EXPECT_CALL(*mockEvent, publishEvent(::testing::_)).Times(0);
    auto eventCreator = [mockEvent](const std::string& eventName) -> std::shared_ptr<IAFBApi::IAFBEvent> {
        return mockEvent;
    };

    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Invoke(eventCreator));
    EXPECT_CALL(*mAfbApi, createEvent(::testing::_)).Times(4);

    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto forwarder = createSubscriberForwarder(capability);
    ASSERT_NE(forwarder, nullptr);

    auto nonExistentEvents = std::list<std::string>({"non", "existent", "events"});
    auto itCapability = nonExistentEvents.begin();
    ASSERT_FALSE(forwarder->forwardMessage(*itCapability++, "My-Payload"));
    ASSERT_FALSE(forwarder->forwardMessage(*itCapability++, "My-Payload"));
    ASSERT_FALSE(forwarder->forwardMessage(*itCapability++, "My-Payload"));
}

TEST_F(SubscriberForwarderTest, canNotPublishOnEventCreationFailure) {
    // Fail the event creation and return null event.
    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Return(nullptr));
    EXPECT_CALL(*mAfbApi, createEvent(::testing::_)).Times(4);

    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto forwarder = createSubscriberForwarder(capability);
    ASSERT_NE(forwarder, nullptr);

    auto itCapability = downstreamEvents.begin();
    std::string payload = "The answer to life the universe and everything = 42";
    ASSERT_FALSE(forwarder->forwardMessage(*itCapability++, payload));
    ASSERT_FALSE(forwarder->forwardMessage(*itCapability++, payload));
    itCapability = upstreamEvents.begin();
    ASSERT_FALSE(forwarder->forwardMessage(*itCapability++, payload));
    ASSERT_FALSE(forwarder->forwardMessage(*itCapability++, payload));
}

TEST_F(SubscriberForwarderTest, canPublishEvents) {
    std::shared_ptr<AFBEventMock> mockEvent(new ::testing::StrictMock<AFBEventMock>());
    ON_CALL(*mockEvent, publishEvent(::testing::_)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(*mockEvent, publishEvent(::testing::_)).Times(4);
    auto eventCreator = [mockEvent](const std::string& eventName) -> std::shared_ptr<IAFBApi::IAFBEvent> {
        return mockEvent;
    };

    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Invoke(eventCreator));
    EXPECT_CALL(*mAfbApi, createEvent(::testing::_)).Times(4);

    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto forwarder = createSubscriberForwarder(capability);
    ASSERT_NE(forwarder, nullptr);

    auto itCapability = downstreamEvents.begin();
    std::string payload = "The answer to life the universe and everything = 42";
    ASSERT_TRUE(forwarder->forwardMessage(*itCapability++, payload));
    ASSERT_TRUE(forwarder->forwardMessage(*itCapability++, payload));
    itCapability = upstreamEvents.begin();
    ASSERT_TRUE(forwarder->forwardMessage(*itCapability++, payload));
    ASSERT_TRUE(forwarder->forwardMessage(*itCapability++, payload));
}

}  // namespace test
}  // namespace vshl