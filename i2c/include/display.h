#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include <stdint.h>
#include "i2c.h"

// Opaque display port structure (hides the structure from the user).
typedef struct display_t display_t;
// Handle (pointer) to the display.
typedef display_t * display_handle_t;

display_handle_t display_init(i2c_handle_t i2c_handle, uint8_t i2c_address);
void command(uint8_t const command);
void data(uint8_t data);

void display_init_task(void *args __attribute((unused)));

#endif /* INCLUDE_DISPLAY_H_ */
