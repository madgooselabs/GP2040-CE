#include "GPGFX.h"

#include <cstring>

#include "drivers/obd_ssd1306.h"

GPGFX::GPGFX() {

}

void GPGFX::init(GPGFX_DisplayTypeOptions* options) {
    if (options->displayType != GPGFX_DisplayType::TYPE_NONE) {
        switch (options->displayType) {
            case GPGFX_DisplayType::TYPE_SSD1306:
                this->displayDriver = new GPGFX_OBD_SSD1306();
                break;
        }
        this->displayDriver->init(options);
    }
}

void GPGFX::clearScreen() {
    this->displayDriver->clear();
}

void GPGFX::drawText(uint16_t x, uint16_t y, std::string text) {
    this->displayDriver->drawText(x, y, text);
}

void GPGFX::drawEllipse(uint16_t x, uint16_t y, uint32_t radiusX, uint32_t radiusY, uint32_t color, uint8_t filled) {
    this->displayDriver->drawEllipse(x, y, radiusX, radiusY, color, filled);
}

void GPGFX::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, uint8_t filled) {
    this->displayDriver->drawLine(x1, y1, x2, y2, color, filled);
}

void GPGFX::drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, uint8_t filled) {
    this->displayDriver->drawRectangle(x, y, width, height, color, filled);
}

void GPGFX::drawSprite(uint8_t* spriteData, uint16_t width, uint16_t height, uint16_t pitch, uint16_t x, uint16_t y, uint8_t priority) {
    this->displayDriver->drawSprite(spriteData, width, height, pitch, x, y, priority);
}