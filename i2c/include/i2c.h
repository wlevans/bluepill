#ifndef __BLUEPILL_I2C__
#define __BLUEPILL_I2C__

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>

void i2c1_init(void);
inline void i2c_reset(uint32_t i2c);
void i2c_write(uint32_t i2c, uint8_t * buffer, size_t length);
void i2c_read(uint32_t i2c, uint8_t * buffer, size_t length);

void i2c_write_task(void *args __attribute((unused)));
void i2c_read_task(void *args __attribute((unused)));

#endif /* __BLUEPILL_I2C__ */
