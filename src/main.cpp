#include <Arduino.h>
#include <WebServer.h>
#include <LEDController.h>
#include <WiFiManager.h>

// WiFI Manager
WiFiManager wm;

// put function declarations here:
bool animationRunning = false;

void setup() {
  Serial.begin(115200);
  
  // Set Up WiFi
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP        
  wm.setHostname("Inifnity Mirror");
  // reset settings - wipe credentials for testing
  // wm.resetSettings();

  wm.setConfigPortalBlocking(false);
  wm.setConfigPortalTimeout(60);
  
  // Automatically connect using saved credentials if they exist
  // If connection fails it starts an access point with the specified name
  if(wm.autoConnect("Unconnected Infinity Mirror")) {
    Serial.println("connected...yeey :)");
  }
  else {
    Serial.println("Configportal running");
  }


  // Initalise the LED Strip and set the default colour to Red
  initStrip();
  changeCylonColour(HtmlColor(0x7f0000));
}

void loop() {
  // Process the WiFi Manager Captive Portal
  wm.process();
  
  
  animationSelector(6);
  animationRunning = true;
}
