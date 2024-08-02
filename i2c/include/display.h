#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include <stdint.h>
#include "i2c.h"

typedef struct
{
    i2c_handle_t * i2c_handle;
} display_handle_t;

void display_init(display_handle_t * display_handle, i2c_handle_t * i2c_handle);
void command(display_handle_t * display_handle, uint8_t const command);
void data(display_handle_t * display_handle, uint8_t data);

void display_init_task(void * args);

#endif /* INCLUDE_DISPLAY_H_ */
