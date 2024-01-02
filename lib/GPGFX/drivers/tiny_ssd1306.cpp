#include "tiny_ssd1306.h"

void GPGFX_TinySSD1306::init(GPGFX_DisplayTypeOptions* options) {
    _options = options;

	printf("GPGFX_TinySSD1306::init\n");

	uint8_t commands[] = {
		0x00,
		CommandOps::DISPLAY_OFF,
		CommandOps::SET_LOW_COLUMN, 0x00,
		CommandOps::SET_HIGH_COLUMN, 0x00,
		CommandOps::SET_START_LINE,
		CommandOps::MEMORY_MODE, 0x00,
		CommandOps::SET_CONTRAST, 0xff,
		CommandOps::NORMAL_DISPLAY,
		CommandOps::SET_MULTIPLEX, 0x3f,
		CommandOps::SET_DISPLAY_OFFSET, 0x00,
		CommandOps::SET_DISPLAY_CLOCK_DIVIDE, 0x80,
		CommandOps::SET_PRECHARGE, 0xF1,
		CommandOps::SET_COM_PINS, 0x12,
		CommandOps::SET_VCOM_DETECT, 0x40,
		CommandOps::CHARGE_PUMP, 0x14,
		CommandOps::FULL_DISPLAY_ON_RESUME,
		CommandOps::DISPLAY_ON,
		CommandOps::DEACTIVATE_SCROLL, 0x00,
		((options->orientation == 1) ? CommandOps::SEGMENT_REMAP_0 : CommandOps::SEGMENT_REMAP_127),
		((options->orientation == 1) ? CommandOps::COM_SCAN_NORMAL : CommandOps::COM_SCAN_REVERSE),
		CommandOps::COLUMN_ADDRESS, 0, (((uint8_t)_metrics->width)-1),
		CommandOps::PAGE_ADDRESS, 0, 7
		//0xae, 0x00 , 0x00 , 0x10 , 0x00 , 0x40 , 0x20 , 0x00 , 0x81 , 0xff , 0xa6 , 0xa8 , 0x3f , 0xd3 , 0x00 , 0xd5 , 0x80 , 0xd9 , 0xf1 , 0xda , 0x12 , 0xdb , 0x40 , 0x8d , 0x14 , 0xa4 , 0xaf , 0x2e , 0x00 , 0xa1 , 0xc8 , 0x21 , 0x00 , 0x7f , 0x22 , 0x00 , 0x07
	};

	sendCommands(commands, sizeof(commands));

	clear();
	drawPixel(5, 5, 1);
	drawBuffer(NULL);
}

void GPGFX_TinySSD1306::setPower(bool isPowered) {
	_isPowered = isPowered;
}

void GPGFX_TinySSD1306::clear() {
	memset(frameBuffer, 0, MAX_SCREEN_SIZE);
}

void GPGFX_TinySSD1306::drawPixel(uint8_t x, uint8_t y, uint32_t color) {
	// return if position out of bounds
	if ((x < 0) || (x >= ((uint8_t)_metrics->width)) || (y < 0) || (y >= ((uint8_t)_metrics->height))) return;

	// byte to be used for buffer operation
	uint8_t byte;

	// display with 32 px height requires doubling of set bits, reason to this is explained in readme
	// this shifts 1 to byte based on y coordinate
	// remember that buffer is a one dimension array, so we have to calculate offset from coordinates
	byte = 1 << (y & 7);

	frameBuffer[x + (y / 8) + ((uint8_t)_metrics->width)] |= byte;
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

void GPGFX_TinySSD1306::drawSprite(uint8_t* image, uint16_t width, uint16_t height, uint16_t pitch, uint16_t anchorX, uint16_t anchorY, uint8_t priority) {

}

void GPGFX_TinySSD1306::drawBuffer(uint8_t* pBuffer) {
	printf("GPGFX_TinySSD1306::drawBuffer(start)\n");
	uint16_t bufferSize = 31;
	uint8_t drawBuffer[bufferSize+1] = {SET_START_LINE};

	int result = -1;
	
	uint8_t commands[] = {0x00, 0xB0 | framePage, 0x00, 0x10};
	sendCommands(commands,sizeof(commands));

	for (uint8_t row = 0; row <= (MAX_SCREEN_WIDTH/bufferSize); row++) {
		if (pBuffer == NULL) {
			//printf("GPGFX_TinySSD1306::drawBuffer(param)\n");
			memcpy(&drawBuffer[1],&frameBuffer[row * bufferSize],bufferSize);
		} else {
			//printf("GPGFX_TinySSD1306::drawBuffer(no param)\n");
			memcpy(&drawBuffer[1],&pBuffer[row * bufferSize],bufferSize);
		}
		result = _options->i2c->write(_options->address, drawBuffer, sizeof(drawBuffer), true);
		printf("GPGFX_TinySSD1306::drawBuffer(i2c):%d\n",result);
	}
	result = _options->i2c->write(_options->address, drawBuffer, 5, true);
	printf("GPGFX_TinySSD1306::drawBuffer(i2c):%d\n",result);

	if (framePage < MAX_SCREEN_HEIGHT/8) {
		framePage++;
	} else {
		framePage = 0;
	}
	sleep_ms(100);

	printf("GPGFX_TinySSD1306::drawBuffer(end)\n");
}

void GPGFX_TinySSD1306::sendCommand(uint8_t command){ 
	uint8_t commandData[] = {0x00, command};
	printf("GPGFX_TinySSD1306::sendCommand(%02x)\n", command);
	sendCommands(commandData, 2);
}

void GPGFX_TinySSD1306::sendCommands(uint8_t* commands, uint16_t length){ 
	int result = _options->i2c->write(_options->address, commands, length, true);
}
