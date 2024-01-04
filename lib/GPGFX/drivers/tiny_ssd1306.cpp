#include "tiny_ssd1306.h"

void GPGFX_TinySSD1306::init(GPGFX_DisplayTypeOptions options) {
    _options.displayType = options.displayType;
    _options.i2c = options.i2c;
    _options.spi = options.spi;
    _options.size = options.size;
    _options.address = options.address;
    _options.orientation = options.orientation;
    _options.inverted = options.inverted;
    _options.font = options.font;

	//printf("GPGFX_TinySSD1306::init\n");

	uint8_t commands[] = {
		0x00,
		CommandOps::DISPLAY_OFF,
		CommandOps::SET_LOW_COLUMN,
		CommandOps::SET_HIGH_COLUMN,
		CommandOps::SET_START_LINE,

		CommandOps::MEMORY_MODE,
		0x00,

		CommandOps::SET_CONTRAST,
		0xFF,

		(!_options.inverted ? CommandOps::NORMAL_DISPLAY : CommandOps::INVERT_DISPLAY),

		CommandOps::SET_MULTIPLEX,
		63,

		CommandOps::SET_DISPLAY_OFFSET,
		0x00,

		CommandOps::SET_DISPLAY_CLOCK_DIVIDE,
		0x80,

		CommandOps::SET_PRECHARGE,
		0x22,

		CommandOps::SET_COM_PINS,
		0x12,

		CommandOps::SET_VCOM_DETECT,
		0x40,

		CommandOps::CHARGE_PUMP,
		0x14,

		(_options.orientation == 1 ? CommandOps::SEGMENT_REMAP_0 : CommandOps::SEGMENT_REMAP_127),
		(_options.orientation == 1 ? CommandOps::COM_SCAN_NORMAL : CommandOps::COM_SCAN_REVERSE),

		CommandOps::FULL_DISPLAY_ON_RESUME,
		CommandOps::DISPLAY_ON
	};

	sendCommands(commands, sizeof(commands));

	clear();
	//drawPixel(5, 5, 1);
	drawBuffer(NULL);
}

void GPGFX_TinySSD1306::setPower(bool isPowered) {
	_isPowered = isPowered;
}

void GPGFX_TinySSD1306::clear() {
	memset(frameBuffer, 0, MAX_SCREEN_SIZE);
}

void GPGFX_TinySSD1306::drawPixel(uint8_t x, uint8_t y, uint32_t color) {
	uint16_t by, bi;

	if ((x<MAX_SCREEN_WIDTH) and (y<MAX_SCREEN_HEIGHT))
	{
		by=((y/8)*128)+x;
		bi=y % 8;

		frameBuffer[by] |= (color<<bi);
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
	long x1 = -radiusX, y1 = 0;
	long e2 = radiusY, dx = (1 + 2 * x1) * e2 * e2;
	long dy = x1 * x1, err = dx + dy;
	long diff = 0;

	while (x1 <= 0) {
		drawPixel(x - x1, y + y1, color);
		drawPixel(x - x1, y + y1, color);
		drawPixel(x + x1, y + y1, color);
		drawPixel(x + x1, y - y1, color);
		drawPixel(x - x1, y - y1, color);

		if (filled)
		{
			for (int i = 0; i < ((x - x1) - (x + x1)) / 2; i++) {
				drawPixel(x - i, y + y1, color);
				drawPixel(x + i, y + y1, color);
				drawPixel(x + i, y - y1, color);
				drawPixel(x - i, y - y1, color);
			}
		}

		e2 = 2 * err;

		if (e2 >= dx) {
			x1++;
			err += dx += 2 * (long)radiusY * radiusY;
		}

		if (e2 <= dy) {
			y1++;
			err += dy += 2 * (long)radiusX * radiusX;
		}
	};

	while (y1++ < radiusY) {
		drawPixel(x, y + y1, color);
		drawPixel(x, y - y1, color);
	}
}

void GPGFX_TinySSD1306::drawRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint32_t color, uint8_t filled) {
	drawLine(x, y, x, height, color, filled);
	drawLine(x, y, width, y, color, filled);
	drawLine(width, height, x, height, color, filled);
	drawLine(width, height, width, y, color, filled);

	if (filled) {
		for (uint8_t i = 1; i < (height-y); i++) {
			drawLine(x, y+i, width, y+i, color, filled);
		}
	}
}

void GPGFX_TinySSD1306::drawSprite(uint8_t* image, uint16_t width, uint16_t height, uint16_t pitch, uint16_t anchorX, uint16_t anchorY, uint8_t priority) {

}

void GPGFX_TinySSD1306::drawBuffer(uint8_t* pBuffer) {
	//printf("GPGFX_TinySSD1306::drawBuffer(start)\n");
	uint16_t bufferSize = MAX_SCREEN_SIZE;
	uint8_t buffer[bufferSize+1] = {SET_START_LINE};

	int result = -1;
	
	//uint8_t commands[] = {0x00, (uint8_t)(0xB0 | framePage), 0x00, 0x10};
	//uint8_t commands[] = {
	//	0x00, 
	//	CommandOps::PAGE_ADDRESS, 0x00, 0x07,
	//	CommandOps::COLUMN_ADDRESS, 0x00, 0x7F
	//};
	//sendCommands(commands,sizeof(commands));
	sendCommand(CommandOps::PAGE_ADDRESS);
	sendCommand(0x00);
	sendCommand(0x07);
	sendCommand(CommandOps::COLUMN_ADDRESS);
	sendCommand(0x00);
	sendCommand(0x7F);
	if (pBuffer == NULL) {
		memcpy(&buffer[1],frameBuffer,bufferSize);
	} else {
		memcpy(&buffer[1],pBuffer,bufferSize);
	}
	result = _options.i2c->write(_options.address, buffer, sizeof(buffer), false);

	//for (int i = 1; i < sizeof(buffer); i++) {
	//	printf("%02x ", buffer[i]);
	//}
	//printf("\n------------\n");
	//printf("GPGFX_TinySSD1306::drawBuffer(i2c):%d\n",result);

	//sendCommands(commands,sizeof(commands));

	//for (uint8_t row = 0; row < (MAX_SCREEN_WIDTH/bufferSize); row++) {
	//	if (pBuffer == NULL) {
	//		//printf("GPGFX_TinySSD1306::drawBuffer(param)\n");
	//		memcpy(&drawBuffer[1],&frameBuffer[row * bufferSize],bufferSize);
	//	} else {
	//		//printf("GPGFX_TinySSD1306::drawBuffer(no param)\n");
	//		memcpy(&drawBuffer[1],&pBuffer[row * bufferSize],bufferSize);
	//	}
	//	result = _options->i2c->write(_options->address, drawBuffer, sizeof(drawBuffer), true);
	//	printf("GPGFX_TinySSD1306::drawBuffer(i2c):%d\n",result);
	//}
	//result = _options->i2c->write(_options->address, drawBuffer, 5, true);
	//printf("GPGFX_TinySSD1306::drawBuffer(i2c):%d\n",result);

	if (framePage < MAX_SCREEN_HEIGHT/8) {
		framePage++;
	} else {
		framePage = 0;
	}

	//sleep_us(100);

	//printf("GPGFX_TinySSD1306::drawBuffer(end)\n");
}

void GPGFX_TinySSD1306::sendCommand(uint8_t command){ 
	uint8_t commandData[] = {0x00, command};
	//printf("GPGFX_TinySSD1306::sendCommand(%02x)\n", command);
	sendCommands(commandData, 2);
}

void GPGFX_TinySSD1306::sendCommands(uint8_t* commands, uint16_t length){ 
	//printf("GPGFX_TinySSD1306::sendCommands() addr: %d\n", _options.address);
	int result = _options.i2c->write(_options.address, commands, length, false);
}
