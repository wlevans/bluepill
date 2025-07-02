#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include <stdint.h>
#include "i2c.h"

#define DISPLAY_ADDRESS 0x3C
#define COMMAND 0x00
#define DATA    0x40

typedef enum
{
  DISPLAY_ERROR_OK = 0,
  DISPLAY_ERROR_INVALID,
} display_error_t;

display_error_t display_init(i2c_interface_t * i2c_interface, uint8_t i2c_address);
void display_init_task(void * args);
void display_task(void * args);

#endif /* INCLUDE_DISPLAY_H_ */
