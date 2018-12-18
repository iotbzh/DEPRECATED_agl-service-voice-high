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

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace vshl {
namespace test {

typedef std::shared_ptr<std::unordered_set<std::string>> WakeWords;

struct VoiceAgentTestData {
    std::string id;
    std::string name;
    std::string description;
    std::string api;
    std::string vendor;
    std::string activeWakeword;
    bool isActive;
    WakeWords wakewords;
};

static std::vector<VoiceAgentTestData> getVoiceAgentsTestData() {
    std::vector<VoiceAgentTestData> voiceAgentsTestData{
        {
            "VA-001",                           // Id
            "Foundation",                       // Name
            "Voice Agent For Galactic Empire",  // Description
            "api-1",                            // API
            "Asimov",                           // Vendor
            "Hari Seldon",                      // Active Wakeword
            true,                               // Is Active
            std::shared_ptr<std::unordered_set<std::string>>(
                new std::unordered_set<std::string>{"Hari Seldon", "Cleon I ", "Eto Demerzel"})  // Wake Words
        },
        {
            "VA-002",                         // Id
            "Betelgeuse",                     // Name
            "Voice Agent For Galaxy hopper",  // Description
            "api-2",                          // API
            "Douglas Adams",                  // Vendor
            "Ford Prefect",                   // Active Wakeword
            true,                             // Is Active
            std::shared_ptr<std::unordered_set<std::string>>(
                new std::unordered_set<std::string>{"Ford Prefect", "Zaphod Beeblebrox"})  // Wake Words
        },
    };

    return voiceAgentsTestData;
}

}  // namespace test
}  // namespace vshl
