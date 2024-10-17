#include <Arduino.h>
#include <WebServer.h>
#include <LEDController.h>

// put function declarations here:
bool animationRunning = false;

void setup() {
  initStrip();
  changeCylonColour(HtmlColor(0x7f0000));
}

void loop() {
    animationSelector(6);
    animationRunning = true;
}
