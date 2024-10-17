#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>

// Function to Initalise the NeoPixel Strip, found in Setup function in Examples
void initStrip();
void SetRandomSeed();

// Function to select which Animation should be played
void animationSelector(int selectedAnimation);

// Cylon Eye Colour Picker
void changeCylonColour(RgbColor eyeColour);

/**/


#endif