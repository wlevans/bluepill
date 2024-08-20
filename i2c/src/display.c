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
	uint8_t commands[] =
	       {COMMAND, 0x2A, 0x71,
			DATA, 0x00,
			COMMAND, 0x28, 0x08, 0x2A, 0x79, 0xD5, 0x70, 0x78, 0x09, 0x06, 0x72,
			DATA, 0x00,
			COMMAND, 0x2A, 0x79, 0xDA, 0x10, 0xDC, 0x00, 0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0x78, 0x28, 0x01, 0x80, 0x0C};
	command(commands, 3);
	data(commands + 3, 1);
	command(commands + 5, 11);
	data(commands + 16, 1);
	command(commands + 18, 18);
	uint8_t setup[] =
	       {COMMAND, 0x01, 0x02, 0xE0,
	        DATA, 0x93, 0x20, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x69, 0x6C, 0x6C, 0x79, 0x20, 0x93};
	command(setup, 4);
	data(setup + 4, 16);

	// Delete this task.
	vTaskDelete(NULL);
}

void command(uint8_t const * command, size_t length)
{
	// Send tx_buffer to I2C send (write) task.
	i2c_write(I2C1, 0x3C, command, length);
	return;
}

void data(uint8_t const * command, size_t length)
{
	// Send tx_buffer to I2C send (write) task;
	i2c_write(I2C1, 0x3C, command, length);
	return;
}
