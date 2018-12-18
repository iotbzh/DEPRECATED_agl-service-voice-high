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

#include "capabilities/core/include/PublisherForwarder.h"
#include "capabilities/core/include/SubscriberForwarder.h"

#include "test/common/ConsoleLogger.h"
#include "test/mocks/AFBApiMock.h"
#include "test/mocks/AFBEventMock.h"
#include "test/mocks/CapabilityMock.h"

using namespace vshl::common::interfaces;
using namespace vshl::capabilities::core;
using namespace vshl::test::common;

namespace vshl {
namespace test {

class PublisherForwarderTest : public ::testing::Test {
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

    std::shared_ptr<PublisherForwarder> createPublisherForwarder(
        std::shared_ptr<::testing::StrictMock<CapabilityMock>> capability) {
        return PublisherForwarder::create(mConsoleLogger, capability);
    }

    std::shared_ptr<::testing::StrictMock<AFBApiMock>> mAfbApi;
    std::shared_ptr<ConsoleLogger> mConsoleLogger;
    std::function<std::shared_ptr<IAFBApi::IAFBEvent>(const std::string&)> mEventCreatorFn;
    std::shared_ptr<::testing::StrictMock<AFBEventMock>> mAfbEventMock;
};

TEST_F(PublisherForwarderTest, failsInitializationOnInvalidParams) {
    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();

    auto forwarder = PublisherForwarder::create(mConsoleLogger, nullptr);
    ASSERT_EQ(forwarder, nullptr);

    forwarder = PublisherForwarder::create(nullptr, capability);
    ASSERT_EQ(forwarder, nullptr);
}

TEST_F(PublisherForwarderTest, initializesCorrectly) {
    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    auto forwarder = createPublisherForwarder(capability);

    ASSERT_NE(forwarder, nullptr);
}

TEST_F(PublisherForwarderTest, canForwardMessages) {
    std::shared_ptr<AFBEventMock> mockEvent(new ::testing::StrictMock<AFBEventMock>());
    ON_CALL(*mockEvent, publishEvent(::testing::_)).WillByDefault(::testing::Return(true));
    EXPECT_CALL(*mockEvent, publishEvent(::testing::_)).Times(4);
    auto eventCreator = [mockEvent](const std::string& eventName) -> std::shared_ptr<IAFBApi::IAFBEvent> {
        return mockEvent;
    };

    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Invoke(eventCreator));
    EXPECT_CALL(*mAfbApi, createEvent(::testing::_)).Times(4);

    auto capability = std::make_shared<::testing::StrictMock<CapabilityMock>>();
    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    auto publisherForwarder = createPublisherForwarder(capability);
    ASSERT_NE(publisherForwarder, nullptr);

    auto subscriberForwarder = createSubscriberForwarder(capability);
    ASSERT_NE(subscriberForwarder, nullptr);

    std::string payload = "The answer to life the universe and everything = 42";
    publisherForwarder->setSubscriberForwarder(subscriberForwarder);

    auto itCapability = downstreamEvents.begin();
    ASSERT_TRUE(publisherForwarder->forwardMessage(*itCapability++, payload));
    ASSERT_TRUE(publisherForwarder->forwardMessage(*itCapability++, payload));
    itCapability = upstreamEvents.begin();
    ASSERT_TRUE(publisherForwarder->forwardMessage(*itCapability++, payload));
    ASSERT_TRUE(publisherForwarder->forwardMessage(*itCapability++, payload));
}

}  // namespace test
}  // namespace vshl