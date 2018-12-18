var afbVshl;
var ws;
var evtIdx = 0;
var count = 0;

var amazon = new AMAZON();
var amazonCbl;

//**********************************************
// Logger
//**********************************************
var log = {
  command: function (url, api, verb, query) {
    console.log("subscribe api=" + api + " verb=" + verb + " query=", query);
    var question = url + "/" + api + "/" + verb + "?query=" + JSON.stringify(query);
    log._write("question", count + ": " + log.syntaxHighlight(question));
  },

  event: function (obj) {
    console.log("gotevent:" + JSON.stringify(obj));
    log._write("outevt", (evtIdx++) + ": " + JSON.stringify(obj));
  },

  reply: function (obj) {
    console.log("replyok:" + JSON.stringify(obj));
    log._write("output", count + ": OK: " + log.syntaxHighlight(obj));
  },

  error: function (obj) {
    console.log("replyerr:" + JSON.stringify(obj));
    log._write("output", count + ": ERROR: " + log.syntaxHighlight(obj));
  },

  _write: function (element, msg) {
    var el = document.getElementById(element);
    el.innerHTML += msg + '\n';

    // auto scroll down
    setTimeout(function () {
      el.scrollTop = el.scrollHeight;
    }, 100);
  },

  syntaxHighlight: function (json) {
    if (typeof json !== 'string') {
      json = JSON.stringify(json, undefined, 2);
    }
    json = json.replace(/&/g, '&amp;').replace(/</g, '&lt;').replace(/>/g, '&gt;');
    return json.replace(/("(\\u[a-zA-Z0-9]{4}|\\[^u]|[^\\"])*"(\s*:)?|\b(true|false|null)\b|-?\d+(?:\.\d*)?(?:[eE][+\-]?\d+)?)/g, function (match) {
      var cls = 'number';
      if (/^"/.test(match)) {
        if (/:$/.test(match)) {
          cls = 'key';
        } else {
          cls = 'string';
        }
      } else if (/true|false/.test(match)) {
        cls = 'boolean';
      } else if (/null/.test(match)) {
        cls = 'null';
      }
      return '<span class="' + cls + '">' + match + '</span>';
    });
  },
};

//**********************************************
// Generic function to call binder
//***********************************************
function callbinder(url, api, verb, query) {
  log.command(url, api, verb, query);

  // ws.call return a Promise
  return ws.call(api + '/' + verb, query)
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


//**********************************************
// connect - establish Websocket connection
//**********************************************

function connect(elemID, api, verb, query) {
  connectVshl(elemID, api, verb, query);
}

function connectVshl(elemID, api, verb, query) {

  function onopen() {
    document.getElementById("main").style.visibility = "visible";
    document.getElementById("connected").innerHTML = "Binder WS Active";
    document.getElementById("connected").style.background = "lightgreen";
    ws.onevent("*", log.event);
    // Fetch and render voice agents.
    fetchAndRenderVoiceAgents();
  }

  function onabort() {
    document.getElementById("main").style.visibility = "hidden";
    document.getElementById("connected").innerHTML = "Connected Closed";
    document.getElementById("connected").style.background = "red";
  }

  var urlparams = {
    base: "api",
    token: "HELLO",
  };
  const vshlAddressInput = document.getElementById('vshl-address');
  urlparams.host = vshlAddressInput.value;

  afbVshl = new AFB(urlparams, "HELLO");
  ws = new afbVshl.ws(onopen, onabort);
}

function clearPre(preId) {
  const pre = document.getElementById(preId);
  while (pre && pre.firstChild) {
    pre.removeChild(pre.firstChild);
  }
}

function fetchAndRenderVoiceAgents() {
  const agentsDiv = document.getElementById('agentsDiv');
  while (agentsDiv.firstChild) {
    agentsDiv.removeChild(agentsDiv.firstChild);
  }

  const api = 'vshl';
  const verb = 'enumerateVoiceAgents';
  const query = {};

  log.command(afbVshl.url, api, verb, query);

  return ws.call(api + '/' + verb, query)
    .then(function (res) {
      log.reply(res);
      for (let index = 0; index < res.response.agents.length; ++index) {
        let voiceAgent = res.response.agents[index];
        addVoiceAgent(agentsDiv, voiceAgent, res.response.default == voiceAgent.id);
      }
    })
    .catch(function (err) {
      log.reply(err);
      console.log(JSON.stringify(err));
    });
}

function addVoiceAgent(containerDiv, voiceAgent, isDefault) {
  const agentDiv = document.createElement("div");

  const agentName = document.createElement("h2");
  agentName.innerHTML = voiceAgent.name;
  agentDiv.appendChild(agentName);

  const agentDescription = document.createElement("p");
  agentDescription.innerHTML = voiceAgent.description;
  agentDiv.appendChild(agentDescription);

  if (!isDefault) {
    const setDefaultBtn = document.createElement("button");
    setDefaultBtn.addEventListener('click', (evt) => {
      const query = {"id": voiceAgent.id};
      callbinder(afbVshl.url, 'vshl', 'setDefaultVoiceAgent', query);
      fetchAndRenderVoiceAgents();
    });
    setDefaultBtn.innerHTML = 'SetDefault';
    agentDiv.appendChild(setDefaultBtn);
  }

  const subscribeBtn = document.createElement("button");
  subscribeBtn.addEventListener('click', (evt) => {
    showAgentEventChooserDialog(voiceAgent.id);
  });
  subscribeBtn.innerHTML = 'Subscribe';
  agentDiv.appendChild(subscribeBtn);

  // Login implementation for Alexa Voice Agent
  if (voiceAgent.name == "Alexa") {
    amazonCbl = new amazon.cbl();
    if (typeof(Storage) !== "undefined" &&
        localStorage.getItem("access_token") !== null &&
        localStorage.getItem("refresh_token") !== null) {
      amazonCbl.refreshToken(voiceAgent);
    } else {
      const loginWithAmazonBtn = document.createElement("button");
      loginWithAmazonBtn.addEventListener('click', (evt) => {
        loginWithAmazonBtn.style.visibility = "hidden";
        amazonCbl.login(voiceAgent);
      });
      loginWithAmazonBtn.innerHTML = 'Login With Amazon!!';
      agentDiv.appendChild(loginWithAmazonBtn);
    }
  }

  containerDiv.appendChild(agentDiv);
}

function showAgentEventChooserDialog(voiceAgentId) {
  const modal = document.getElementById('agent-event-chooser');
  const subscribeBtn = document.getElementById('agent-subscribe-btn');

  subscribeBtn.addEventListener('click', (evt) => {
    const authState = document.getElementById('authstate').checked;
    const dialogState = document.getElementById('dialogstate').checked;
    const connectionState = document.getElementById('connectionstate').checked;

    const query = {
      "va_id": voiceAgentId,
      "events":[]
    };
    if (authState)
      query.events.push('voice_authstate_event');
    if (dialogState)
      query.events.push('voice_dialogstate_event');
    if (connectionState)
      query.events.push('voice_connectionstate_event');

    callbinder(afbVshl.url, 'vshl', 'subscribe', query);
    modal.close();
  });

  // makes modal appear (adds `open` attribute)
  modal.showModal();
}

function startListening() {
    callbinder(afbVshl.url, 'vshl', 'startListening', {});  
}