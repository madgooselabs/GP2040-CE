#include "MainMenuScreen.h"

extern uint32_t getMillis();

void MainMenuScreen::drawScreen() {
    getRenderer()->drawText(1, 1, "GPGFX_UI Test Menu");

    for (size_t i = 0; i < currentMenu->size(); ++i) {
        MenuEntry entry = currentMenu->at(i);

        getRenderer()->drawText(3, 3+i, entry.label);
    }

    if (!isPressed) {
        if (pressedUp()) {
            if (menuIndex > 0) {
                menuIndex--;
            } else {
                menuIndex = currentMenu->size()-1;
            }
            checkDebounce = getMillis();
            isPressed = true;
        } else if (pressedDown()) {
            if (menuIndex < currentMenu->size()-1) {
                menuIndex++;
            } else {
                menuIndex = 0;
            }
            checkDebounce = getMillis();
            isPressed = true;
        } else if (pressedB1()) {
            currentMenu->at(menuIndex).action();
            checkDebounce = getMillis();
            isPressed = true;
        }
    } else {
        if (isPressed && ((getMillis() - checkDebounce) > 400)) {
            isPressed = false;
        }
    }

    getRenderer()->drawText(1, 3+menuIndex, ">");
}

void MainMenuScreen::setMenu(std::vector<MenuEntry>* menu) {
    currentMenu = menu;
}