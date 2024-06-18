#ifndef __BLUEPILL_I2C__
#define __BLUEPILL_I2C__

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/i2c.h>

void i2c_init(void);

#endif /* __BLUEPILL_I2C__ */
