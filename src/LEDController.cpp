#include <LEDController.h>

// NeoPixel Setup
const uint16_t PixelCount = 45; // make sure to set this to the number of pixels in your strip
const uint16_t PixelPin = 5;  // make sure to set this to the correct pin, ignored for Esp8266
NeoPixelBus<NeoGrbFeature, NeoWs2812xMethod> strip(PixelCount, PixelPin);

// Initialise Strip
void initStrip(){
    SetRandomSeed();
    strip.Begin();
    strip.Show();
}

void SetRandomSeed() {
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(16);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(16) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}


/* ANIMATION 1 - BASIC ANIMATION */
NeoPixelAnimator basicAnimations(PixelCount, NEO_CENTISECONDS);

void SetupAnimationSet() {
    // setup some animations
    for (uint16_t pixel = 0; pixel < PixelCount; pixel++) {
        const uint8_t peak = 128;

        // pick a random duration of the animation for this pixel
        // since values are centiseconds, the range is 1 - 4 seconds
        uint16_t time = random(100, 400);

        // each animation starts with the color that was present
        RgbColor originalColor = strip.GetPixelColor<RgbColor>(pixel);
        // and ends with a random color
        RgbColor targetColor = RgbColor(random(peak), random(peak), random(peak));
        // with the random ease function
        AnimEaseFunction easing;

        switch (random(3)) {
        case 0:
            easing = NeoEase::CubicIn;
            break;
        case 1:
            easing = NeoEase::CubicOut;
            break;
        case 2:
            easing = NeoEase::QuadraticInOut;
            break;
        }
        // we must supply a function that will define the animation, in this example
        // we are using "lambda expression" to define the function inline, which gives
        // us an easy way to "capture" the originalColor and targetColor for the call back.
        //
        // this function will get called back when ever the animation needs to change
        // the state of the pixel, it will provide a animation progress value
        // from 0.0 (start of animation) to 1.0 (end of animation)
        //
        // we use this progress value to define how we want to animate in this case
        // we call RgbColor::LinearBlend which will return a color blended between
        // the values given, by the amount passed, which is also a float value from 0.0-1.0.
        // then we set the color.
        //
        // There is no need for the MyAnimationState struct as the compiler takes care
        // of those details for us
        AnimUpdateCallback animUpdate = [=](const AnimationParam& param) {
            // progress will start at 0.0 and end at 1.0
            // we convert to the curve we want
            float progress = easing(param.progress);

            // use the curve value to apply to the animation
            RgbColor updatedColor = RgbColor::LinearBlend(originalColor, targetColor, progress);
            strip.SetPixelColor(pixel, updatedColor);
        };

        // now use the animation properties we just calculated and start the animation
        // which will continue to run and call the update function until it completes
        basicAnimations.StartAnimation(pixel, time, animUpdate);
    }
}


/* ANIMATION 2 - FADE IN FADE OUT */
NeoPixelAnimator fadeInFadeOutAnimations(1); // NeoPixel animation management object
boolean fadeToColor = true;  // general purpose variable used to store effect state

// what is stored for state is specific to the need, in this case, the colors.
// basically what ever you need inside the animation update function
struct FadeInFadeOutAminationState {
    RgbColor StartingColor;
    RgbColor EndingColor;
};

// one entry per pixel to match the animation timing manager
FadeInFadeOutAminationState fadeInFadeOutAnimationState[1];
// simple blend function
void SimpleBlendAnimUpdate(const AnimationParam& param) {
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        fadeInFadeOutAnimationState[param.index].StartingColor,
        fadeInFadeOutAnimationState[param.index].EndingColor,
        param.progress
    );

    // apply the color to the strip
    for (uint16_t pixel = 0; pixel < PixelCount; pixel++) {
        strip.SetPixelColor(pixel, updatedColor);
    }
}

void FadeInFadeOutRinseRepeat(float luminance) {
    if (fadeToColor) {
        // Fade upto a random color
        // we use HslColor object as it allows us to easily pick a hue
        // with the same saturation and luminance so the colors picked
        // will have similiar overall brightness
        RgbColor target = HslColor(random(360) / 360.0f, 1.0f, luminance);
        uint16_t time = random(800, 2000);

        fadeInFadeOutAnimationState[0].StartingColor = strip.GetPixelColor<RgbColor>(0);
        fadeInFadeOutAnimationState[0].EndingColor = target;

        fadeInFadeOutAnimations.StartAnimation(0, time, SimpleBlendAnimUpdate);
    }
    else {
        // fade to black
        uint16_t time = random(600, 700);

        fadeInFadeOutAnimationState[0].StartingColor = strip.GetPixelColor<RgbColor>(0);
        fadeInFadeOutAnimationState[0].EndingColor = RgbColor(0);

        fadeInFadeOutAnimations.StartAnimation(0, time, SimpleBlendAnimUpdate);
    }

    // toggle to the next effect state
    fadeToColor = !fadeToColor;
}


/* AMIMATION 3 - FUN RANDOM CHANGE */
NeoPixelAnimator fRCAnimations(PixelCount);

struct FRCAnimationState {
    RgbColor StartingColor;
    RgbColor EndingColor;
};

// one entry per pixel to match the animation timing manager
FRCAnimationState fRCAnimationState[PixelCount];

// simple blend function
void FRCBlendAnimUpdate(const AnimationParam& param) {
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        fRCAnimationState[param.index].StartingColor,
        fRCAnimationState[param.index].EndingColor,
        param.progress);
    // apply the color to the strip
    strip.SetPixelColor(param.index, updatedColor);
}

void PickRandom(float luminance){ 
    // pick random count of pixels to animate
    uint16_t count = random(PixelCount);
    while (count > 0) {
        // pick a random pixel
        uint16_t pixel = random(PixelCount);

        // pick random time and random color
        // we use HslColor object as it allows us to easily pick a color
        // with the same saturation and luminance 
        uint16_t time = random(100, 400);
        fRCAnimationState[pixel].StartingColor = strip.GetPixelColor<RgbColor>(pixel);
        fRCAnimationState[pixel].EndingColor = HslColor(random(360) / 360.0f, 1.0f, luminance);

        fRCAnimations.StartAnimation(pixel, time, FRCBlendAnimUpdate);

        count--;
    }
}


/* ANIMATION 4 - ROTATE LOOP */
const uint16_t AnimCount = 1; // we only need one
const uint16_t TailLength = 6; // length of the tail, must be shorter than PixelCount
const float MaxLightness = 0.4f; // max lightness at the head of the tail (0.5f is full bright)
NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma

NeoPixelAnimator rotateLoopAnimations(AnimCount); 

void LoopAnimUpdate(const AnimationParam& param) {
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed) {
        // done, time to restart this position tracking animation/timer
        rotateLoopAnimations.RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        strip.RotateRight(1);
    }
}

void DrawTailPixels() {
    // using Hsl as it makes it easy to pick from similiar saturated colors
    float hue = random(360) / 360.0f;
    for (uint16_t index = 0; index < strip.PixelCount() && index <= TailLength; index++) {
        float lightness = index * MaxLightness / TailLength;
        RgbColor color = HslColor(hue, 1.0f, lightness);
        strip.SetPixelColor(index, colorGamma.Correct(color));
    }
}


/* ANIMATION 5 - CYLON */
NeoPixelAnimator cylonAnimations(2);
uint16_t lastPixel = 0; // track the eye position
int8_t moveDir = 1; // track the direction of movement
// uncomment one of the lines below to see the effects of changing the ease function on the movement animation
AnimEaseFunction moveEase =
//      NeoEase::Linear;
//      NeoEase::QuadraticInOut;
//      NeoEase::CubicInOut;
        NeoEase::QuarticInOut;
//      NeoEase::QuinticInOut;
//      NeoEase::SinusoidalInOut;
//      NeoEase::ExponentialInOut;
//      NeoEase::CircularInOut;

RgbColor CylonEyeColor;
void changeCylonColour(RgbColor eyeColour) {
    CylonEyeColor = eyeColour;
}


void FadeAll(uint8_t darkenBy) {
    RgbColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip.PixelCount(); indexPixel++) {
        color = strip.GetPixelColor<RgbColor>(indexPixel);
        color.Darken(darkenBy);
        strip.SetPixelColor(indexPixel, color);
    }
}

void FadeAnimUpdate(const AnimationParam& param) {
    if (param.state == AnimationState_Completed) {
        FadeAll(10);
        cylonAnimations.RestartAnimation(param.index);
    }
}

void MoveAnimUpdate(const AnimationParam& param) {
    // apply the movement animation curve
    float progress = moveEase(param.progress);

    // use the curved progress to calculate the pixel to effect
    uint16_t nextPixel;
    if (moveDir > 0) {
        nextPixel = progress * strip.PixelCount();
    }
    else {
        nextPixel = (1.0f - progress) * strip.PixelCount();
    }

    // if progress moves fast enough, we may move more than
    // one pixel, so we update all between the calculated and
    // the last
    if (lastPixel != nextPixel) {
        for (uint16_t i = lastPixel + moveDir; i != nextPixel; i += moveDir) {
            strip.SetPixelColor(i, CylonEyeColor);
        }
    }
    strip.SetPixelColor(nextPixel, CylonEyeColor);

    lastPixel = nextPixel;

    if (param.state == AnimationState_Completed) {
        // reverse direction of movement
        moveDir *= -1;

        // done, time to restart this position tracking animation/timer
        cylonAnimations.RestartAnimation(param.index);
    }
}

void SetupCylonAnimations() {
    // fade all pixels providing a tail that is longer the faster
    // the pixel moves.
    cylonAnimations.StartAnimation(0, 5, FadeAnimUpdate);

    // take several seconds to move eye fron one side to the other
    cylonAnimations.StartAnimation(1, 2000, MoveAnimUpdate);
}


/* ANIMATION 6 - FUN ROTATE LOOP */
const uint16_t FunLoopAnimCount = PixelCount / 5 * 2 + 1; // we only need enough animations for the tail and one extra
const uint16_t FunLoopPixelFadeDuration = 300; // third of a second
// one second divide by the number of pixels = loop once a second
const uint16_t FunLoopNextPixelMoveDuration = 1000 / PixelCount; // how fast we move through the pixels

struct FunLoopAnimationState {
    RgbColor StartingColor;
    RgbColor EndingColor;
    uint16_t IndexPixel; // which pixel this animation is effecting
};

NeoPixelAnimator funLoopAnimations(FunLoopAnimCount); // NeoPixel animation management object
FunLoopAnimationState funLoopAnimationState[FunLoopAnimCount];
uint16_t frontPixel = 0;  // the front of the loop
RgbColor frontColor;  // the color at the front of the loop

void FadeOutAnimUpdate(const AnimationParam& param) {
    // this gets called for each animation on every time step
    // progress will start at 0.0 and end at 1.0
    // we use the blend function on the RgbColor to mix
    // color based on the progress given to us in the animation
    RgbColor updatedColor = RgbColor::LinearBlend(
        funLoopAnimationState[param.index].StartingColor,
        funLoopAnimationState[param.index].EndingColor,
        param.progress
    );
    // apply the color to the strip
    strip.SetPixelColor(
        funLoopAnimationState[param.index].IndexPixel, 
        colorGamma.Correct(updatedColor)
    );
}

void FunLoopAnimUpdate(const AnimationParam& param) {
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed) {
        // done, time to restart this position tracking animation/timer
        funLoopAnimations.RestartAnimation(param.index);

        // pick the next pixel inline to start animating
        // 
        frontPixel = (frontPixel + 1) % PixelCount; // increment and wrap
        if (frontPixel == 0) {
            // we looped, lets pick a new front color
            frontColor = HslColor(random(360) / 360.0f, 1.0f, 0.25f);
        }

        uint16_t indexAnim;
        // do we have an animation available to use to animate the next front pixel?
        // if you see skipping, then either you are going to fast or need to increase
        // the number of animation channels
        if (funLoopAnimations.NextAvailableAnimation(&indexAnim, 1)) {
            funLoopAnimationState[indexAnim].StartingColor = frontColor;
            funLoopAnimationState[indexAnim].EndingColor = RgbColor(0, 0, 0);
            funLoopAnimationState[indexAnim].IndexPixel = frontPixel;

            funLoopAnimations.StartAnimation(indexAnim, FunLoopPixelFadeDuration, FadeOutAnimUpdate);
        }
    }
}



/* ANIMATION SELECTOR FUNCTION */
void animationSelector(int selectedAnimation) {
    if (selectedAnimation == 0) {
        // Stop All Animations before starting new one
        basicAnimations.StopAll();
        fadeInFadeOutAnimations.StopAll();
        fRCAnimations.StopAll();
        rotateLoopAnimations.StopAll();
        cylonAnimations.StopAll();
        funLoopAnimations.StopAll();
    }

    else if (selectedAnimation == 1) {
        if (basicAnimations.IsAnimating()) {
            // the normal loop just needs these two to run the active animations
            basicAnimations.UpdateAnimations();
            strip.Show();
        }
        else {
            SetupAnimationSet();
        }
    }

    else if (selectedAnimation == 2) {
        if (fadeInFadeOutAnimations.IsAnimating()) {
            // the normal loop just needs these two to run the active animations
            fadeInFadeOutAnimations.UpdateAnimations();
            strip.Show();
        }
        else {
            // no animation runnning, start some
            FadeInFadeOutRinseRepeat(0.2f); // 0.0 = black, 0.25 is normal, 0.5 is bright
        }
    }

    else if (selectedAnimation == 3) {
        if (fRCAnimations.IsAnimating()) {
            // the normal loop just needs these two to run the active animations
            fRCAnimations.UpdateAnimations();
            strip.Show();
        }
        else {
            // no animations runnning, start some 
            PickRandom(0.2f); // 0.0 = black, 0.25 is normal, 0.5 is bright
        }
    }

    else if (selectedAnimation == 4) {
        if (rotateLoopAnimations.IsAnimating()) {
            rotateLoopAnimations.UpdateAnimations();
            strip.Show();
        }
        else {
            // Draw the tail that will be rotated through all the rest of the pixels
            DrawTailPixels();
            // we use the index 0 animation to time how often we rotate all the pixels
            rotateLoopAnimations.StartAnimation(0, 66, LoopAnimUpdate); 
        }
    }

    else if (selectedAnimation == 5) {
        if (cylonAnimations.IsAnimating()) {
            cylonAnimations.UpdateAnimations();
            strip.Show();
        }
        else {
            SetupCylonAnimations();
        }
    }

    else if (selectedAnimation == 6){
        if (funLoopAnimations.IsAnimating()) {
            funLoopAnimations.UpdateAnimations();
            strip.Show();
        }
        else {
            funLoopAnimations.StartAnimation(0, FunLoopNextPixelMoveDuration, FunLoopAnimUpdate);
        }
    }
}