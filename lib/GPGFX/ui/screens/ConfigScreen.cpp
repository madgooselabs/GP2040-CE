#include "ConfigScreen.h"

#include "version.h"

void ConfigScreen::drawScreen() {
	//drawStatusBar(gamepad);
	getRenderer()->drawText(0, 1, "[Web Config Mode]");
	getRenderer()->drawText(0, 2, std::string("GP2040-CE : ") + std::string(GP2040VERSION));
	getRenderer()->drawText(0, 3, "[http://192.168.7.1]");
	getRenderer()->drawText(0, 4, "Preview:");
	getRenderer()->drawText(5, 5, "B1 > Button");
	getRenderer()->drawText(5, 6, "B2 > Splash");

    getRenderer()->drawText(0, 0, header);
    getRenderer()->drawText(0, 50, footer);
}
