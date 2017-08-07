#include "mcp23017.h"
#include "TWIlib.h"
#include <util/delay.h>

#define MCP23017_BASE_ADDRESS 0x20 // default address when all three address pins are grounded on the chip
#define MCP23017_READ_ADDRESS  (MCP23017_BASE_ADDRESS << 1) | 0x01 // address shifted and last bit set
#define MCP23017_WRITE_ADDRESS (MCP23017_BASE_ADDRESS << 1) & 0xFE // address shifted and last bit cleared

void mcp23017_init(void)
{
	mcp23017_write_register(MCP23017_IODIRA,   0xFF);	// Set all pins to input
	mcp23017_write_register(MCP23017_IODIRB,   0xFF);	// Set all pins to input
	mcp23017_write_register(MCP23017_GPPUA,    0xFF);	// Set 100k pullup on all pins
	mcp23017_write_register(MCP23017_GPPUB,    0xFF);	// Set 100k pullup on all pins
	mcp23017_write_register(MCP23017_IPOLA,    0xFF);	// Invert polarity of button signal, if taken to ground, GPIO shows logical 1
	mcp23017_write_register(MCP23017_IPOLB,    0xFF);	// Invert polarity of button signal, if taken to ground, GPIO shows logical 1
	mcp23017_write_register(MCP23017_IOCONA,   0x60);	// Configure the interrupt system (mirror INTA/B, INTA/B not floating, interrupt signaled with LOW) 
	mcp23017_write_register(MCP23017_IOCONB,   0x60);	// Configure the interrupt system (mirror INTA/B, INTA/B not floating, interrupt signaled with LOW) 
	//mcp23017_write_register(MCP23017_DEFVALA,  0xFF);	// Default value of pins
	//mcp23017_write_register(MCP23017_DEFVALB,  0xFF);	// Default value of pins
	//mcp23017_write_register(MCP23017_INTCONA,  0xFF);	// Compare interrupt to the default value
	//mcp23017_write_register(MCP23017_INTCONB,  0xFF);	// Compare interrupt to the default value
	mcp23017_write_register(MCP23017_GPINTENA, 0xFF);	// Enable interrupts on all pins
	mcp23017_write_register(MCP23017_GPINTENB, 0xFF);	// Enable interrupts on all pins
}

void mcp23017_write_register(uint8_t reg, uint8_t data)
{
	uint8_t temp[3];
	temp[0] = MCP23017_WRITE_ADDRESS;
	temp[1] = reg;
	temp[2] = data;

	// Set the error code to have no relevant information
	TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
	// Continuously attempt to transmit data until a successful transmission occurs
	while (TWIInfo.errorCode != 0xFF)
	{
		TWITransmitData(temp, 3, 0);
	}
}

uint8_t mcp23017_read_register(uint8_t reg)
{
	uint8_t temp[2];
	temp[0] = MCP23017_WRITE_ADDRESS;
	temp[1] = reg;

	// Set the error code to have no relevant information
	TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
	// Continuously attempt to transmit data until a successful transmission occurs
	while (TWIInfo.errorCode != 0xFF)
	{
		TWITransmitData(temp, 2, 1);
	}

	// Now that the data has been written, another write operation is required to set the
	// address to read from. A Repeated START is used to maintain control of the bus.
	// This is important as if another Master takes control of the EEPROM and changes its address
	// before we get to read the data, then data will be read from the wrong address
	
	TWIInfo.errorCode = TWI_NO_RELEVANT_INFO;
	while (TWIInfo.errorCode != 0xFF)
	{
		TWIReadData(MCP23017_BASE_ADDRESS, 1, 0); // the read-bit is added inside the function!
	}

	while (isTWIReady() == 0) {_delay_ms(1);}

	return TWIReceiveBuffer[0];
}
