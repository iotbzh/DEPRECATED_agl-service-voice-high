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

#include "capabilities/CapabilityMessagingService.h"

#include "test/common/ConsoleLogger.h"
#include "test/mocks/AFBApiMock.h"
#include "test/mocks/AFBEventMock.h"
#include "test/mocks/AFBRequestMock.h"
#include "test/mocks/CapabilityMock.h"

using namespace vshl::common::interfaces;
using namespace vshl::capabilities;
using namespace vshl::test::common;

namespace vshl {
namespace test {

class CapabilityMessagingServiceTest : public ::testing::Test {
protected:
    void SetUp() override {
        mConsoleLogger = std::make_shared<ConsoleLogger>();
        mAfbApi = std::make_shared<::testing::NiceMock<AFBApiMock>>();
    }

    std::shared_ptr<::testing::NiceMock<AFBApiMock>> mAfbApi;
    std::shared_ptr<ConsoleLogger> mConsoleLogger;
};

TEST_F(CapabilityMessagingServiceTest, failsInitializationOnInvalidParams) {
    auto service = CapabilityMessagingService::create(mConsoleLogger, nullptr);
    ASSERT_EQ(service, nullptr);

    service = CapabilityMessagingService::create(nullptr, mAfbApi);
    ASSERT_EQ(service, nullptr);
}

TEST_F(CapabilityMessagingServiceTest, initializesSuccessfully) {
    auto service = CapabilityMessagingService::create(mConsoleLogger, mAfbApi);
    ASSERT_NE(service, nullptr);
}

TEST_F(CapabilityMessagingServiceTest, canSubscribeAndPublishCapabilityEvents) {
    auto service = CapabilityMessagingService::create(mConsoleLogger, mAfbApi);

    auto capability = std::make_shared<::testing::NiceMock<CapabilityMock>>();
    std::list<std::string> upstreamEvents({"up-ev1", "up-ev2"});
    std::list<std::string> downstreamEvents({"down-ev1", "down-ev2"});
    std::string capabilityName = "weather";

    ON_CALL(*capability, getName()).WillByDefault(::testing::Return(capabilityName));
    ON_CALL(*capability, getUpstreamMessages()).WillByDefault(::testing::Return(upstreamEvents));
    ON_CALL(*capability, getDownstreamMessages()).WillByDefault(::testing::Return(downstreamEvents));

    // We don't care if and how many times subscribe method is called on event.
    std::shared_ptr<AFBEventMock> mockEvent(new ::testing::NiceMock<AFBEventMock>());
    ON_CALL(*mockEvent, subscribe(::testing::_)).WillByDefault(::testing::Return(true));
    ON_CALL(*mockEvent, publishEvent(::testing::_)).WillByDefault(::testing::Return(true));
    auto eventCreator = [mockEvent](const std::string& eventName) -> std::shared_ptr<IAFBApi::IAFBEvent> {
        mockEvent->setName(eventName);
        return mockEvent;
    };

    ON_CALL(*mAfbApi, createEvent(::testing::_)).WillByDefault(::testing::Invoke(eventCreator));

    auto request = std::make_shared<::testing::StrictMock<AFBRequestMock>>();
    std::string payload = "The answer to life the universe and everything = 42";

    bool result = service->publish(capability, *upstreamEvents.begin(), payload);
    ASSERT_FALSE(result);  // Without subscribing to the event. Publish should fail.

    result = service->subscribe(*request, capability, *upstreamEvents.begin());
    ASSERT_TRUE(result);

    result = service->subscribe(*request, capability, *downstreamEvents.begin());
    ASSERT_TRUE(result);

    result = service->publish(capability, *downstreamEvents.begin(), payload);
    ASSERT_TRUE(result);
}

}  // namespace test
}  // namespace vshl