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
#ifndef VSHL_CAPABILITIES_PHONECONTROL_MESSAGES_H_
#define VSHL_CAPABILITIES_PHONECONTROL_MESSAGES_H_

#include <list>
#include <string>

using namespace std;

namespace vshl {
namespace capabilities {
namespace phonecontrol {

static string NAME = "phonecontrol";

// Supported actions from VA -> Apps
/* Dial message sent from VA to app handling the calling.
 *
 * Payload
 *  {
 *     "callId": "{{STRING}}",
 *     "callee": {
 *       "details": "{{STRING}}",
 *       "defaultAddress": {
 *         "protocol": "{{STRING}}",
 *         "format": "{{STRING}}",
 *         "value": "{{STRING}}"
 *       },
 *       "alternativeAddresses": [{
 *         "protocol": "{{STRING}}",
 *         "format": "{{STRING}}",
 *         "value": {{STRING}}
 *       }]
 *     }
 *   }
 * }
 *
 * callId (required): A unique identifier for the call
 * callee (required): The destination of the outgoing call
 * callee.details (optional): Descriptive information about the callee
 * callee.defaultAddress (required): The default address to use for calling the callee
 * callee.alternativeAddresses (optional): An array of alternate addresses for the existing callee
 * address.protocol (required): The protocol for this address of the callee (e.g. PSTN, SIP, H323, etc.)
 * address.format (optional): The format for this address of the callee (e.g. E.164, E.163, E.123, DIN5008, etc.)
 * address.value (required): The address of the callee.
 *
 */
static string PHONECONTROL_DIAL = "phonecontrol/dial";
/**
 * Notifies the platform implementation to redial the last called phone number.
 * 
 * After returning @c true, if no stored number is available to be redialed, @c PhoneCallController::callFailed with 
 * @c CallError::NO_NUMBER_FOR_REDIAL should be called.
 * 
 * @param [in] payload Details of the redial request in structured JSON format. See the following
 * payload structure and the description of each field:
 * @code{.json}
 * {
 *   "callId": "{{STRING}}"
 * }
 * @endcode 
 * @li callId (required): A unique identifier for the call
 *
 * @return @c true if the platform implementation successfully handled the call
 */ 
static string PHONECONTROL_REDIAL = "phonecontrol/redial";
/**
 * Notifies the platform implementation to answer an inbound call
 * 
 * @param [in] payload Details of the answer request in structured JSON format. See the following
 * payload structure and the description of each field:
 * @code{.json}
 * {
 *   "callId": "{{STRING}}",
 * }
 * @endcode 
 * @li callId (required): The unique identifier for the call to answer
 */  
static string PHONECONTROL_ANSWER = "phonecontrol/answer";
/**
 * Notifies the platform implementation to end an ongoing call or stop inbound or outbound call setup
 * 
 * @param [in] payload Details of the stop request in structured JSON format. See the following
 * payload structure and the description of each field:
 * @code{.json}
 * {
 *   "callId": "{{STRING}}"
 * }
 * @endcode 
 * @li callId (required): The unique identifier for the call to be stopped
 */
static string PHONECONTROL_STOP = "phonecontrol/stop";
/**
 * Notifies the platform implementation to send a DTMF signal to the calling device
 * 
 * @param [in] payload Details of the DTMF request in structured JSON format. See the following
 * payload structure and the description of each field:
 * @code{.json}
 * {
 *   "callId": "{{STRING}}",
 *   "signal": "{{STRING}}"
 * }
 * @endcode 
 * @li callId (required): The unique identifier for the call
 * @li signal (required): The DTMF string to be sent to the calling device associated with the callId
 */ 
static string PHONECONTROL_SEND_DTMF = "phonecontrol/send_dtmf";


// Supported actions from Apps -> VA
/*
 *  App notifies the voiceagents of a change in connection state of a calling device.
 *
 *  Payload
 *  {
 *	    "state" : "{{STRING}}" // CONNECTED or DISCONNECTED
 *  }
 */
static string PHONECONTROL_CONNECTIONSTATE_CHANGED = "phonecontrol/connection_state_changed";
/*
 *  App notifies the voiceagents that call is activated
 *
 *  callId must match the one that is sent by VA with DIAL message above.
 *
 *  Payload
 *  {
 *	    "callId" : "{{STRING}}"
 *  }
 */
static string PHONECONTROL_CALL_STATE_CHANGED = "phonecontrol/call_state_changed";
/*
 *  App notifies the voiceagents of an error in initiating or maintaining a
 *  call on a calling device
 *
 *  callId must match the one that is sent by VA with DIAL message above.
 *  error: below status codes.
 *     4xx: Validation failure for the input from the DIAL message
 *     500: Internal error on the platform unrelated to the cellular network
 *     503: Error on the platform related to the cellular network
 *
 *  Payload
 *  {
 *	    "callId" : "{{STRING}}"
 *      "error"  : "{{STRING}}"
 *  }
 */
static string PHONECONTROL_CALL_FAILED = "phonecontrol/call_failed";
/**
  * App notifies the voiceagents that a caller id was received for an inbound call
  * 
  * @param [in] callId The unique identifier for the call associated with the callId
  * @param [in] callerId The caller's identifier or phone number
  */
static string PHONECONTROL_CALLERID_RECEIVED = "phonecontrol/caller_id_received";

/** 
 * Notifies the Engine that sending the DTMF signal succeeded.
 * 
 * @param [in] callId The unique identifier for the associated call
 * 
 * @sa PhoneCallController::sendDTMF
 */
static string PHONECONTROL_SEND_DTMF_SUCCEEDED = "phonecontrol/send_dtmf_succeeded";

// List of actions that are delivered from VA -> Apps
static list<string> PHONECONTROL_UPSTREAM_ACTIONS = {
    PHONECONTROL_DIAL,
    PHONECONTROL_REDIAL,
    PHONECONTROL_ANSWER,
    PHONECONTROL_STOP,
    PHONECONTROL_SEND_DTMF
};

// List of actions that are delivered from Apps -> VA
static list<string> PHONECONTROL_DOWNSTREAM_ACTIONS = {PHONECONTROL_CONNECTIONSTATE_CHANGED,
                                                       PHONECONTROL_CALL_STATE_CHANGED,
                                                       PHONECONTROL_CALL_FAILED,
                                                       PHONECONTROL_CALLERID_RECEIVED,
                                                       PHONECONTROL_SEND_DTMF_SUCCEEDED};

}  // namespace phonecontrol
}  // namespace capabilities
}  // namespace vshl

#endif  // VSHL_CAPABILITIES_PHONECONTROL_MESSAGES_H_
