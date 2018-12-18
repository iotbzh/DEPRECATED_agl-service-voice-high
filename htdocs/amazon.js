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
AMAZON = function() {

var afb;
var alexaWs;

var base = {
  base: "api",
  token: "HELLO",
};

// GUID generator for generating device serial number.
function guid() {
  function s4() {
    return Math.floor((1 + Math.random()) * 0x10000)
      .toString(16)
      .substring(1);
  }
  return s4() + s4() + '-' + s4() + '-' + s4() + '-' + s4() + '-' + s4() + s4() + s4();
}

/*********************************************/
/****                                     ****/
/****             AMAZON_cbl              ****/
/****                                     ****/
/*********************************************/
var AMAZON_Cbl;
{
  const amazonHostUrl = "https://api.amazon.com";
  const amazonCodePairUrl = amazonHostUrl + "/auth/O2/create/codepair";
  const amazonTokenUrl    = amazonHostUrl + "/auth/O2/token";
  const deviceSerialNumber = guid();
  var clientID = "amzn1.application-oa2-client.1675c236e59546c2bea62ea5444d3510"; // localStorage.getItem("client_id");
  var productID = "Test"; // localStorage.getItem("product_id");
  var alexaVAAddress = localStorage.getItem("alexa_va_address");
  var alexaVAConnected = false;
  var alexaVAAddressInput;

  AMAZON_Cbl = function() {
    // Alexa VA Address
    const alexaVAAddressInput = document.getElementById('alexa-va-address');
    alexaVAAddress = alexaVAAddressInput.value;
    connectToAlexaVA(alexaVAAddress);

    alexaVAAddressInput.addEventListener("change",(evt) => {
      var newAlexaVAAddress = alexaVAAddressInput.value;
      if (alexaVAAddress != newAlexaVAAddress) {
        connectToAlexaVA(newAlexaVAAddress);
        localStorage.setItem("alexa_va_address", newAlexaVAAddress);
      }
    });
  }

  function connectToAlexaVA(address) {
    base.host = address;
    afb = new AFB(base, "HELLO");

    function onopen() {
      console.log("Connected to Alexa VA");
      alexaVAConnected = true;
    }

    function onabort() {
      console.log("Alexa VA connection aborted.");
      alexaVAConnected = false;
    }

    alexaWs = new afb.ws(onopen, onabort);
  }

  function sendRequest(httpReq, paramsJson, url, responseCb) {
    httpReq.onreadystatechange = responseCb;
    var paramsQueryString = Object.keys(paramsJson).map(key => key + '=' + paramsJson[key]).join('&');
    httpReq.open("POST", url, true);
    httpReq.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    httpReq.send(paramsQueryString);
  }

  //**********************************************
  // Generic function to call VA binder
  //***********************************************
  function callVABinder(voiceAgent, verb, query) {
    console.log(voiceAgent.api, verb, query);

    // ws.call return a Promise
    return alexaWs.call(voiceAgent.api + '/' + verb, query)
      .then(function (res) {
        log.reply(res);
        count++;
        return res;
      })
      .catch(function (err) {
        log.reply(err);
        count++;
        throw err;
      });
  };

  function updateAccessToken(voiceAgent, tokenResponseJson) {
    if (alexaVAAddress === undefined || alexaVAAddress === null) {
      console.log("No Alexa VA. So not updating the access token.");
      return;
    }

    // store the access and refresh tokens.
    if (typeof(Storage) !== "undefined") {
      localStorage.setItem("access_token", tokenResponseJson["access_token"]);
      localStorage.setItem("refresh_token", tokenResponseJson["refresh_token"]);
    }

    // Set the auth token
    if (alexaVAConnected) {
      // Set new token
      const query = {"token": tokenResponseJson["access_token"]};
      callVABinder(voiceAgent, 'setAuthToken', query);
    }

    // Refresh the token as soon as it expires.
    setTimeout(refreshToken, tokenResponseJson["expires_in"] * 1000);
  }

  function refreshToken(voiceAgent) {
    if (voiceAgent == "undefined") {
      console.log("Error: VoiceAgent undefined");
      return;
    }

    var refreshToken = localStorage.getItem("refresh_token");
    if (refreshToken == null) {
      console.log("Error: No refresh token");
      return;
    }

    var paramsJson = {
      "grant_type":"refresh_token",
      "refresh_token":refreshToken,
      "client_id":clientID,
    };

    const tokenRefreshReq = new XMLHttpRequest();
    sendRequest(tokenRefreshReq, paramsJson, amazonTokenUrl, function() {
      if (tokenRefreshReq.readyState == 4) {
        if (tokenRefreshReq.status == 200) {
          console.log("Got access token " + tokenRefreshReq.responseText);
          var tokenResponseJson = JSON.parse(tokenRefreshReq.responseText);
          updateAccessToken(voiceAgent, tokenResponseJson);
        } else {
          console.log("Failed to refresh access token: " + tokenRefreshReq.responseText);
        }
      }
    });
  }

  function displayUserCodeAndURI(authResponseJson) {
    const modal = document.getElementById('login-with-amazon');
    const cblStatusDiv = document.createElement("div");
    const cblStatusMsg = document.createElement("p");
    const blank = "_blank";

    var cblPage = authResponseJson["verification_uri"] + "?cbl-code=" + authResponseJson["user_code"]
    var msg = "To use Alexa,you must sign in to Amazon.<br> Go to " +
        "<a href=" + cblPage + "  target="+ blank+ " >" +
        cblPage + "</a>";
    cblStatusMsg.innerHTML = msg;
    cblStatusDiv.appendChild(cblStatusMsg);
    modal.appendChild(cblStatusDiv);

    const closeBtn = document.createElement("button");
    closeBtn.addEventListener('click', (evt) => {
      modal.close();
    });
    closeBtn.style = "margin: 10px";
    closeBtn.innerHTML = "Close";
    modal.appendChild(closeBtn);
  }

  function hideLoginUI() {
    const loginDiv = document.getElementById('login-area');
    loginDiv.style.display = "none";
  }

  function login(voiceAgent) {
    if (voiceAgent == undefined) {
      console.log("Error: VoiceAgent undefined");
      return;
    }

    const modal = document.getElementById('login-with-amazon');
    const submitBtn = document.getElementById('submit-btn');
    const cancelBtn = document.getElementById('cancel-btn');
    submitBtn.addEventListener('click', (evt) => {
      console.log("Alexa Destination address set to: " + alexaVAAddress);
      startLoginProcess(voiceAgent);
    });

    cancelBtn.addEventListener('click', (evt) => {
      modal.close();
    });

    const alexaVAAddressInput = document.getElementById('alexa-va-address');
    alexaVAAddressInput.value = alexaVAAddress;

    modal.showModal();
  }

  function startLoginProcess(voiceAgent) {
    if (alexaVAAddress == null) {
      console.log("Required information missing to start login process.");
      return;
    }

    var reqJson = {
      "response_type": "device_code",
      "client_id": clientID,
      "scope":"alexa:all",
      "scope_data": JSON.stringify({
        "alexa:all": {
          "productID":productID,
          "productInstanceAttributes" : {
            "deviceSerialNumber": deviceSerialNumber
          }
        }
      })
    };

    const authReq = new XMLHttpRequest();
    var tokenUrl = amazonTokenUrl;
    sendRequest(authReq, reqJson, amazonCodePairUrl, function() {
      if (authReq.readyState == 4) {
        if (authReq.status == 200) {
          var authResponse = JSON.parse(authReq.responseText);
          console.log("Got auth codepair " + authReq.responseText);
          hideLoginUI();
          displayUserCodeAndURI(authResponse);
          var maxTokenReqCnt = authResponse["expires_in"] / authResponse["interval"];
          var tokenReqFuncId = setTimeout(function tokenReqFunc() {
            var reqJson = {
              "grant_type":"device_code",
              "device_code":authResponse["device_code"],
              "user_code":authResponse["user_code"]
            };
            const tokenReq = new XMLHttpRequest();
            sendRequest(tokenReq, reqJson, tokenUrl, function() {
              if (tokenReq.readyState == 4) {
                if (tokenReq.status == 200) {
                  console.log("Got access token " + tokenReq.responseText);
                  var tokenResponseJson = JSON.parse(tokenReq.responseText);
                  updateAccessToken(voiceAgent, tokenResponseJson);
                }
                else {
                  maxTokenReqCnt--;
                  console.log("Retrying... " + tokenReq.responseText);
                  setTimeout(tokenReqFunc, authResponse["interval"] * 1000);
                }
              }
            });
          }, authResponse["interval"] * 1000);
          // Cancel if max token request attempts are reached.
          if (maxTokenReqCnt == 0) {
            console.log("Reached max token request attemps limit.");
          }
        } else {
          console.log(authReq.status);
        }
      }
    });
  }

  AMAZON_Cbl.prototype = {
    login: login,
    refreshToken: refreshToken,
  };
}
/*********************************************/
/****                                     ****/
/****                                     ****/
/****                                     ****/
/*********************************************/
return {
  cbl: AMAZON_Cbl
};
};