#ifndef _SPLASHSCREEN_H_
#define _SPLASHSCREEN_H_

#include "GPGFX_UI_widgets.h"
#include "bitmaps.h"

class SplashScreen : public GPScreen {
    public:
        SplashScreen() {}
        SplashScreen(GPGFX* renderer) { setRenderer(renderer); }
    protected:
        void drawScreen();
};

#endif