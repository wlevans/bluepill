#include "display.h"

#define COMMAND 0x00
#define DATA    0x40

uint8_t tx_buffer[2];

void display_init(void)
{

}
void commmand(uint8_t const * command)
{
	tx_buffer[0] = COMMAND;
	tx_buffer[1] = *command;
	// Send tx_buffer to I2C send (write) task.
	return;
}
void data(uint8_t * data)
{
	tx_buffer[0] = DATA;
	tx_buffer[1] = *data;
	// Send tx_buffer to I2C send (write) task;
	return;
}
