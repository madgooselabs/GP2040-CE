// PCF8575 Library
// category=Signal Input/Output

// Written by:
//   Mike Parks <mikepparks@gmail.com>

#ifndef _PCF8575_H_
#define _PCF8575_H_

#include "peripheral_i2c.h"

class PCF8575  {
  protected:
	uint8_t address;
  public:
    // Constructor 
	PCF8575(PeripheralI2C *i2cController, uint8_t addr = 0x20);

    //// Methods
    void begin();
	void reset();
  	//void start();

    void send(uint16_t value);
    uint16_t receive();

    void setPin(uint8_t pinNumber, uint8_t value);
    bool getPin(uint8_t pinNumber);
  private:
	PeripheralI2C* i2c;
    const uint16_t initialValue = 0xFFFF;
    uint8_t uc[128];

    uint16_t dataSent;
    uint16_t dataReceived;
};

#endif
