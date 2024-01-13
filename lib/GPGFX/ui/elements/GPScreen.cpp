#include "GPScreen.h"

void GPScreen::draw() {
    getRenderer()->clearScreen();

    drawScreen();
    
    getRenderer()->render();
}
