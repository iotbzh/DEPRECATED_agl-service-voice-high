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

#include <iostream>

#include "test/common/ConsoleLogger.h"

namespace vshl {
namespace test {
namespace common {

void ConsoleLogger::log(Level level, const std::string& tag, const std::string& message) {
    string format_msg = "Tag: " + tag + ", message: " + message;
    std::cout << format_msg << std::endl;
}

}  // namespace common
}  // namespace test
}  // namespace vshl
