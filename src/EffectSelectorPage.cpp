#include <EffectSelectorPage.h>


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

const char* PARAM_INPUT_1 = "output";
const char* PARAM_INPUT_2 = "state";

int selectedAnimation = 0;

String outputState(int output) {
    if(digitalRead(output)) {
        return "checked";
    }
    else {
        return "";
    }
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons = "";
    buttons += "<h4>Basic Pattern</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"1\" " + outputState(1) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Fade In Fade Out</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"2\" " + outputState(2) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Random Change</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"3\" " + outputState(3) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Rotating Loop</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"4\" " + outputState(4) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Bounce</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"5\" " + outputState(5) + "><span class=\"slider\"></span></label>";
    buttons += "<h4>Fancy Rotating Loop</h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"6\" " + outputState(6) + "><span class=\"slider\"></span></label>";
    return buttons;
  }
  return String();
}


/* WEB SITE CODE */

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>Infinity Mirror Controller</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <link rel="icon" href="data:,">
  <style>
    html {font-family: Arial; display: inline-block; text-align: center;}
    h2 {font-size: 3.0rem;}
    p {font-size: 3.0rem;}
    body {max-width: 600px; margin:0px auto; padding-bottom: 25px;}
    .switch {position: relative; display: inline-block; width: 120px; height: 68px} 
    .switch input {display: none}
    .slider {position: absolute; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; border-radius: 6px}
    .slider:before {position: absolute; content: ""; height: 52px; width: 52px; left: 8px; bottom: 8px; background-color: #fff; -webkit-transition: .4s; transition: .4s; border-radius: 3px}
    input:checked+.slider {background-color: #b30000}
    input:checked+.slider:before {-webkit-transform: translateX(52px); -ms-transform: translateX(52px); transform: translateX(52px)}
  </style>
</head>
<body>
  <h2>Infinity Mirror Controller</h2>
  %BUTTONPLACEHOLDER%
<script>function toggleCheckbox(element) {
  var xhr = new XMLHttpRequest();
  if(element.checked){ xhr.open("GET", "/update?output="+element.id+"&state=1", true); }
  else { xhr.open("GET", "/update?output="+element.id+"&state=0", true); }
  xhr.send();
}
</script>
<h3>Written by David Eldridge 2024<h3>
</body>
</html>
)rawliteral";




void startWebServer() {
    // Route for root / web page
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", index_html, processor);
    });
    
    // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    server.on(
        "/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
            String inputMessage1;
            String inputMessage2;
            
            // GET input1 value on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
            if (request->hasParam(PARAM_INPUT_1) && request->hasParam(PARAM_INPUT_2)) {
                inputMessage1 = request->getParam(PARAM_INPUT_1)->value();
                inputMessage2 = request->getParam(PARAM_INPUT_2)->value();
                // // Turn off Animations
                if (inputMessage2.toInt() == 0){
                    selectedAnimation = 0;
                }
                else {
                    selectedAnimation = inputMessage1.toInt();
                }
                
            }

            else {
                inputMessage1 = "No message sent";
                inputMessage2 = "No message sent";
            }

            request->send(200, "text/plain", "OK");
        }
    );
    server.begin();
}

int getAnimation(){
    return selectedAnimation;
}