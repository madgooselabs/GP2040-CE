#ifndef _CONFIGSCREEN_H_
#define _CONFIGSCREEN_H_

#include "GPGFX_UI_widgets.h"

class ConfigScreen : public GPScreen {
    public:
        ConfigScreen() {}
        ConfigScreen(GPGFX* renderer) { setRenderer(renderer); }
    protected:
        void drawScreen();
};

#endif