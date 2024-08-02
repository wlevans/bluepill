#ifndef __BLUEPILL_I2C__
#define __BLUEPILL_I2C__

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/i2c.h>
#include "FreeRTOS.h"
#include "queue.h"

#define QUEUE_SIZE 16

typedef enum
{
  I2C_ERROR_OK = 0,
  I2C_ERROR_PORT,
  I2C_ERROR_MODE
} i2c_error_t;

typedef enum
{
  I2C_PORT_1 = 0,
  I2C_PORT_2,
  I2C_PORT_COUNT
} i2c_port_t;

typedef enum
{
  I2C_MODE_STANDARD = 0,
  I2C_MODE_FAST,
  I2C_MODE_COUNT
} i2c_mode_t;

typedef struct
{

  uint8_t * write_buffer;
  uint8_t * read_buffer;
  uint8_t address;
  uint8_t write_count;
  uint8_t read_count;
} i2c_transaction_t;

typedef struct
{
    uint32_t port;
    QueueHandle_t queue;
} i2c_handle_t;

i2c_error_t i2c1_init(i2c_handle_t * i2c_handle, uint32_t i2c_port, i2c_mode_t i2c_mode);
void i2c_write(uint32_t i2c, uint8_t address, uint8_t * buffer, size_t length);
void i2c_read(uint32_t i2c, uint8_t address, uint8_t * buffer, size_t length);

void i2c_transaction_task(void * args);

#endif /* __BLUEPILL_I2C__ */
