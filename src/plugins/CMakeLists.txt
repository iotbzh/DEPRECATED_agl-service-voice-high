###########################################################################
# Copyright 2017-2018 Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
###########################################################################

PROJECT_TARGET_ADD(vshl-api)

    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)

    set(VSHL_LIB_SRC
        ${CMAKE_CURRENT_SOURCE_DIR}/VshlApi.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/VshlApi.h

        # Interfaces
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/afb/IAFBApi.h
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/appmanagement/IAppController.h
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/capabilities/ICapability.h
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/utilities/events/IEventFilter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/utilities/logging/ILogger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/voiceagents/IVoiceAgent.h
        ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/voiceagents/IVoiceAgentsChangeObserver.h

        # AFB
        ${CMAKE_CURRENT_SOURCE_DIR}/afb/AFBApiImpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/afb/AFBApiImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/afb/AFBRequestImpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/afb/AFBRequestImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/afb/include/AFBEventImpl.h
        ${CMAKE_CURRENT_SOURCE_DIR}/afb/src/AFBEventImpl.cpp

        # AppManagement
        ${CMAKE_CURRENT_SOURCE_DIR}/appmanagement/AppController.h
        ${CMAKE_CURRENT_SOURCE_DIR}/appmanagement/AppController.cpp

        # VoiceAgents
        ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/VoiceAgentsDataManager.h
        ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/VoiceAgentsDataManagerImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/VoiceAgentEventNames.h
        ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/include/VoiceAgent.h
        ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/src/VoiceAgentImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/include/VoiceAgentEventsHandler.h
        ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/src/VoiceAgentEventsHandler.cpp

        # Core
        ${CMAKE_CURRENT_SOURCE_DIR}/core/VRRequestProcessor.h
        ${CMAKE_CURRENT_SOURCE_DIR}/core/VRRequestProcessorImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include/VRAgentsObserver.h
        ${CMAKE_CURRENT_SOURCE_DIR}/core/src/VRAgentsObserverImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include/VRRequest.h
        ${CMAKE_CURRENT_SOURCE_DIR}/core/src/VRRequestImpl.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include/VRRequestProcessorDelegate.h
        ${CMAKE_CURRENT_SOURCE_DIR}/core/src/VRRequestProcessorDelegateImpl.cpp

        #Capabilities
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/CapabilitiesFactory.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/CapabilitiesFactory.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/CapabilityMessagingService.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/CapabilityMessagingService.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/core/include/MessageChannel.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/core/src/MessageChannel.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/core/include/PublisherForwarder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/core/src/PublisherForwarder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/core/include/SubscriberForwarder.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/core/src/SubscriberForwarder.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/communication/include/PhoneControlMessages.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/communication/include/PhoneControlCapability.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/communication/src/PhoneControlCapability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/navigation/include/NavigationMessages.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/navigation/include/NavigationCapability.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/navigation/src/NavigationCapability.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/guimetadata/include/GuiMetadataMessages.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/guimetadata/include/GuiMetadataCapability.h
        ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/guimetadata/src/GuiMetadataCapability.cpp

        #Utilities
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities/events/EventRouter.h
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities/events/EventRouter.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities/logging/Logger.h
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities/logging/Logger.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities/uuid/UUIDGeneration.h
        ${CMAKE_CURRENT_SOURCE_DIR}/utilities/uuid/UUIDGeneration.cpp
    )

    # Define targets
    ADD_LIBRARY(${TARGET_NAME} MODULE
        ${VSHL_LIB_SRC}
    )

    # VSHL plugin properties
    SET_TARGET_PROPERTIES(${TARGET_NAME} PROPERTIES
        LABELS "PLUGIN"
        PREFIX ""
        SUFFIX ".ctlso"
        OUTPUT_NAME ${TARGET_NAME}
    )

    # Define target includes
    TARGET_INCLUDE_DIRECTORIES(${TARGET_NAME}
        PUBLIC ${GLIB_PKG_INCLUDE_DIRS}
        PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}"
        PRIVATE "${CMAKE_SOURCE_DIR}/app-controller/ctl-lib"
    )

    # Library dependencies (include updates automatically)
    TARGET_LINK_LIBRARIES(${TARGET_NAME}
        afb-helpers
        ${GLIB_PKG_LIBRARIES}
        ${link_libraries}
    )

    option(ENABLE_UNIT_TESTS "Build unit tests or not" OFF)
    if (ENABLE_UNIT_TESTS)
        execute_process(
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/agreement.sh"
            RESULT_VARIABLE AGREEMENT_RESULT
        )
        message(STATUS "Agreement Result: ${AGREEMENT_RESULT}")
        if (${AGREEMENT_RESULT} MATCHES "1")
            message(FATAL_ERROR "User agreement not accepted. Quitting")
        endif()

        include(cmake/gtest.cmake)

        set(VSHL_TEST_SRC ${VSHL_LIB_SRC})
        list(APPEND VSHL_TEST_SRC
            # Main
            ${CMAKE_CURRENT_SOURCE_DIR}/TestMain.cpp

            # Test common
            ${CMAKE_CURRENT_SOURCE_DIR}/test/common/ConsoleLogger.h
            ${CMAKE_CURRENT_SOURCE_DIR}/test/common/ConsoleLogger.cpp

            # Test Mocks
            ${CMAKE_CURRENT_SOURCE_DIR}/test/mocks/AFBApiMock.h
            ${CMAKE_CURRENT_SOURCE_DIR}/test/mocks/AFBEventMock.h
            ${CMAKE_CURRENT_SOURCE_DIR}/test/mocks/AFBRequestMock.h
            ${CMAKE_CURRENT_SOURCE_DIR}/test/mocks/CapabilityMock.h
            ${CMAKE_CURRENT_SOURCE_DIR}/test/mocks/VoiceAgentsChangeObserverMock.h

            # Capabilities
            ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/test/CapabilityMessagingServiceTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/test/PublisherForwarderTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/capabilities/test/SubscriberForwarderTest.cpp

            # Core
            ${CMAKE_CURRENT_SOURCE_DIR}/core/test/VRRequestTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/core/test/VRRequestProcessorTest.cpp

            # VoiceAgents
            ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/test/VoiceAgentTest.cpp
            ${CMAKE_CURRENT_SOURCE_DIR}/voiceagents/test/VoiceAgentsDataManagerTest.cpp
        )

        ADD_EXECUTABLE(${TARGET_NAME}_Test
            ${VSHL_TEST_SRC}
        )

        TARGET_INCLUDE_DIRECTORIES(${TARGET_NAME}_Test
            PUBLIC ${GLIB_PKG_INCLUDE_DIRS}
            PUBLIC  "${CMAKE_CURRENT_SOURCE_DIR}"
            PRIVATE "${CMAKE_SOURCE_DIR}/app-controller/ctl-lib"
        )

        TARGET_LINK_LIBRARIES(${TARGET_NAME}_Test
            afb-helpers
            libgtest
            libgmock
            ${GLIB_PKG_LIBRARIES}
            ${link_libraries}
        )

        ENABLE_TESTING()
        ADD_TEST(VshlTest ${TARGET_NAME}_Test)
    endif()