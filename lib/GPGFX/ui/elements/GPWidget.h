#ifndef _GPWIDGET_H_
#define _GPWIDGET_H_

#include "GPGFX.h"
#include "GPGFX_UI.h"

class GPWidget : public GPGFX_UI {
    public:
        GPWidget() {}
        GPWidget(GPGFX* renderer) { setRenderer(renderer); }
        virtual void draw() {}
};

#endif