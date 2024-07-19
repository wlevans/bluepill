#ifndef __BLUEPILL_I2C__
#define __BLUEPILL_I2C__

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/i2c.h>


#define NUMBER_OF_PORTS 2

typedef enum
{
	I2C_ERROR_OK = 0,
	I2C_ERROR_PORT,
	I2C_ERROR_MODE
} i2c_error_t;

typedef enum
{
  MODE_STANDARD = 0,
  MODE_FAST
} i2c_mode_t;

i2c_error_t i2c1_init(uint32_t port, i2c_mode_t mode);
void i2c_write(uint32_t i2c, uint8_t address, uint8_t * buffer, size_t length);
void i2c_read(uint32_t i2c, uint8_t address, uint8_t * buffer, size_t length);

void i2c_write_task(void *args __attribute((unused)));
void i2c_read_task(void *args __attribute((unused)));

#endif /* __BLUEPILL_I2C__ */
