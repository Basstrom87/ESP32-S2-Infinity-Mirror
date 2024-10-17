#ifndef EFFECT_SELECTOR_PAGE_H
#define EFFECT_SELECTOR_PAGE_H

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

void startWebServer();

String processor(const String& var);

int getAnimation();


#endif