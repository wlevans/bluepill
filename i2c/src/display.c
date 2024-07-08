#include "display.h"

// To do:
// Remove when ready.
#include "i2c.h"

#define COMMAND 0x00
#define DATA    0x40

uint8_t tx_buffer[2];

void display_init(void)
{
	command(0x2A);
	command(0x71);
	data(0x00);
	command(0x28);
	command(0x08);
	command(0x2A);
	command(0x79);
	command(0xD5);
	command(0x70);
	command(0x78);
	command(0x09);
	command(0x06);
	command(0x72);
	data(0x00);
	command(0x2A);
	command(0x79);
	command(0xDA);
	command(0x10);
	command(0xDC);
	command(0x00);
	command(0x81);
	command(0x7F);
	command(0xD9);
	command(0xF1);
	command(0xDB);
	command(0x40);
	command(0x78);
	command(0x28);
	command(0x01);
	command(0x80);
	command(0x0C);
}
void command(uint8_t const command)
{
	tx_buffer[0] = COMMAND;
	tx_buffer[1] = command;
	// Send tx_buffer to I2C send (write) task.
	i2c_write(I2C1, 0x3C, tx_buffer, 2);
	return;
}
void data(uint8_t data)
{
	tx_buffer[0] = DATA;
	tx_buffer[1] = data;
	// Send tx_buffer to I2C send (write) task;
	i2c_write(I2C1, 0x3C, tx_buffer, 2);
	return;
}
