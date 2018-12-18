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
// This code is copied from https://github.com/alexa/avs-device-sdk/
// Minor modifications are made to the original file.

#ifndef VSHL_UTILITIES_UUID_UUIDGENERATION_H_
#define VSHL_UTILITIES_UUID_UUIDGENERATION_H_

#include <string>

namespace vshl {
namespace utilities {
namespace uuid {

/**
 * Generates a variant 1, version 4 universally unique identifier (UUID) consisting of 32 hexadecimal digits.
 * The UUID generated is of the format xxxxxxxx-xxxx-Mxxx-Nxxx-xxxxxxxxxxxx where M indicates the version, and the two
 * most significant bits of N indicates the variant. M is 0100 (binary) for version 4 and N is 10xx(binary) for
 * variant 1.
 * @see https://tools.ietf.org/html/rfc4122.
 *
 * @return A uuid as a string.
 */
const std::string generateUUID();

}  // namespace uuid
}  // namespace utilities
}  // namespace vshl

#endif  // VSHL_UTILITIES_UUID_UUIDGENERATION_H_