#include "tiny_ssd1306.h"

void GPGFX_TinySSD1306::init(GPGFX_DisplayTypeOptions* options) {
    _options = options;

	uint8_t commands[] = {
		0x00,
		CommandOps::DISPLAY_OFF,
		CommandOps::SET_LOW_COLUMN,
		CommandOps::SET_HIGH_COLUMN,
		CommandOps::SET_START_LINE,
		CommandOps::MEMORY_MODE, 0x00,
		CommandOps::SET_CONTRAST, 0xff,
		CommandOps::NORMAL_DISPLAY,
		CommandOps::SET_MULTIPLEX, 0x3f,
		CommandOps::SET_DISPLAY_OFFSET, 0x00,
		CommandOps::SET_DISPLAY_CLOCK_DIVIDE, 0x80,
		CommandOps::SET_PRECHARGE, 0x22,
		CommandOps::SET_COM_PINS, 0x12,
		CommandOps::SET_VCOM_DETECT, 0x40,
		CommandOps::CHARGE_PUMP, 0x14,
		CommandOps::FULL_DISPLAY_ON_RESUME,
		CommandOps::DISPLAY_ON,
		((options->orientation) ? CommandOps::SEGMENT_REMAP_0 : CommandOps::SEGMENT_REMAP_127),
		((options->orientation) ? CommandOps::COM_SCAN_NORMAL : CommandOps::COM_SCAN_REVERSE),
		CommandOps::COLUMN_ADDRESS, 0, (((uint8_t)_metrics->width)-1),
		CommandOps::PAGE_ADDRESS, 0, 7
	};

	sendCommands(commands, sizeof(commands));

	clear();
	drawBuffer(NULL);
}

void GPGFX_TinySSD1306::setPower(bool isPowered) {
}

void GPGFX_TinySSD1306::clear() {
	memset(frameBuffer, 0, MAX_SCREEN_SIZE);
}

void GPGFX_TinySSD1306::drawPixel(uint8_t x, uint8_t y, uint32_t color) {
	int loc = x + (y / 8) * ((uint8_t)_metrics->width);
	int bitMask = 1 << (y & 7);
	if (color > 0) {
		frameBuffer[loc] |= bitMask;
	} else {
		frameBuffer[loc] &= ~bitMask;
	}
}

void GPGFX_TinySSD1306::drawText(uint8_t x, uint8_t y, std::string text) {
}

void GPGFX_TinySSD1306::drawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint32_t color, uint8_t filled) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int stepX = (x1 < x2) ? 1 : -1;
    int stepY = (y1 < y2) ? 1 : -1;

    int err = dx - dy;

    while (true) {
        drawPixel(x1, y1, color);

        if (x1 == x2 && y1 == y2) break;

        int errDouble = 2 * err;
        if (errDouble > -dy) {
            err -= dy;
            x1 += stepX;
        }
        if (errDouble < dx) {
            err += dx;
            y1 += stepY;
        }
    }
}

void GPGFX_TinySSD1306::drawEllipse(uint16_t x, uint16_t y, uint32_t radiusX, uint32_t radiusY, uint32_t color, uint8_t filled) {
	for(int dy=-radiusY; dy<=radiusY; dy++) {
		for(int dx=-radiusX; dx<=radiusX; dx++) {
			if(dx*dx*radiusY*radiusY+dy*dy*radiusX*radiusX <= radiusY*radiusY*radiusX*radiusX)
				drawPixel(x+dx, y+dy, color);
		}
	}
}

void GPGFX_TinySSD1306::drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, uint8_t filled) {
}

void GPGFX_TinySSD1306::drawSprite(uint8_t* spriteData, uint16_t width, uint16_t height, uint16_t pitch, uint16_t x, uint16_t y, uint8_t priority) {
}

void GPGFX_TinySSD1306::drawBuffer(uint8_t *pBuffer) {
	uint8_t drawBuffer[MAX_SCREEN_SIZE+1] = {0x40};
	memcpy(&drawBuffer[1],frameBuffer,sizeof(frameBuffer));
	sendCommand(CommandOps::COLUMN_ADDRESS);
	sendCommand(0);
	sendCommand(((uint8_t)_metrics->width)-1);
	sendCommand(CommandOps::PAGE_ADDRESS);
	sendCommand(0);
	sendCommand(7);
	int result = _options->i2c->write(_options->address, drawBuffer, sizeof(drawBuffer), false);
}

void GPGFX_TinySSD1306::sendCommand(uint8_t command){ 
	uint8_t commandData[] = {0x00, command};
	sendCommands(commandData, 2);
}

void GPGFX_TinySSD1306::sendCommands(uint8_t* commands, uint16_t length){ 
	int result = _options->i2c->write(_options->address, commands, length, false);
}
