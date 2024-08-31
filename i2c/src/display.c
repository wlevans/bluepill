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

typedef struct
{
    i2c_interface_t * i2c_interface;
    uint8_t i2c_address;
} display_t;

display_error_t display_init(i2c_interface_t * i2c_interface, uint8_t i2c_address)
{
  // Create display structure for initialization task argument.
  display_t * display = (display_t *)pvPortMalloc(sizeof(display_t));
  if(NULL == display)
  {
    return(DISPLAY_ERROR_INVALID);
  }
  display->i2c_interface = i2c_interface;
  display->i2c_address = i2c_address;

  // Create FreeRTOS tasks. This task must have a higher priority than any task
  // that uses the display.
  xTaskCreate(display_init_task, "display_init_task", 100, (void *)display, 1, NULL);

  return(DISPLAY_ERROR_OK);
}

void display_init_task(void * args)
{
  // Get task arguments.
  display_t * display = (display_t *)args;
  i2c_interface_t * i2c_interface = display->i2c_interface;

  uint8_t init_commands_1[] = {COMMAND, 0x2A, 0x71};
  uint8_t init_commands_2[] = {COMMAND, 0x28, 0x08, 0x2A, 0x79, 0xD5, 0x70, 0x78, 0x09, 0x06, 0x72};
  uint8_t init_commands_3[] = {COMMAND, 0x2A, 0x79, 0xDA, 0x10, 0xDC, 0x00, 0x81, 0x7F, 0xD9, 0xF1, 0xDB, 0x40, 0x78, 0x28, 0x01, 0x80, 0x0C};
  uint8_t init_data[] = {DATA, 0x00};

  i2c_interface->write(i2c_interface->port, display->i2c_address, init_commands_1, sizeof(init_commands_1));
  i2c_interface->write(i2c_interface->port, display->i2c_address, init_data, sizeof(init_data));
  i2c_interface->write(i2c_interface->port, display->i2c_address, init_commands_2, sizeof(init_commands_2));
  i2c_interface->write(i2c_interface->port, display->i2c_address, init_data, sizeof(init_data));
  i2c_interface->write(i2c_interface->port, display->i2c_address, init_commands_3, sizeof(init_commands_3));

  // Free up argument.
  vPortFree(args);
  // Delete this task.
  vTaskDelete(NULL);
}
