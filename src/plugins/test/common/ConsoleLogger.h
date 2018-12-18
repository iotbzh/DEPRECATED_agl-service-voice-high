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
#ifndef VSHL_TEST_COMMON_CONSOLE_LOGGER_H_
#define VSHL_TEST_COMMON_CONSOLE_LOGGER_H_

#include "interfaces/utilities/logging/ILogger.h"

using namespace std;

namespace vshl {
namespace test {
namespace common {

class ConsoleLogger : public vshl::common::interfaces::ILogger {
public:
  // ILogger interface
  void log(Level level, const std::string &tag,
           const std::string &message) override;
};

} // namespace common
} // namespace test
} // namespace vshl

#endif // VSHL_TEST_COMMON_CONSOLE_LOGGER_H_
