#include <stdlib.h>
#include "display.h"
#include "FreeRTOS.h"
#include "task.h"

// To do:
// Remove when ready.
#include "i2c.h"

#define COMMAND 0x00
#define DATA    0x40

uint8_t tx_buffer[2];

typedef struct display_t
{
    i2c_handle_t i2c_handle;
    uint8_t i2c_address;
};

display_handle_t display_init(i2c_handle_t i2c_handle, uint8_t i2c_address)
{
  // Create I2C handle to be returned.
	display_handle_t display_handle = (display_handle_t)malloc(sizeof(display_t));
  if(NULL == i2c_handle)
  {
	return NULL;
  }
  display_handle->i2c_handle = i2c_handle;
  display_handle->i2c_address = i2c_address;

  // Create FreeRTOS tasks.
  xTaskCreate(display_init_task, "display_init_task", 100, NULL, configMAX_PRIORITIES - 1, NULL);
  return display_handle;
}

void display_init_task(void *args __attribute((unused)))
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

	{
	  // Set up display.
	  command(0x01);
	  command(0x02);
	  command(0xe0);
	  data(0x93);
	  data(0x20);
	  data(0x48);
	  data(0x65);
	  data(0x6C);
	  data(0x6C);
	  data(0x6F);
	  data(0x20);
	  data(0x57);
	  data(0x69);
	  data(0x6C);
	  data(0x6C);
	  data(0x79);
	  data(0x20);
	  data(0x93);
	}

	// Delete this task.
	vTaskDelete(NULL);
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
