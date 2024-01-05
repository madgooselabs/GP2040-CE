#ifndef _GPGFX_UI_H_
#define _GPGFX_UI_H_

#include <string>

#include "GPGFX_types.h"
#include "drivers/displaybase.h"

#include "fonts/GP_Font_Basic.h"
#include "fonts/GP_Font_Big.h"
#include "fonts/GP_Font_Standard.h"

class GPGFX_UI {
    public:
        GPGFX_UI();

        void init();

        void update();
    private:
};

#endif
