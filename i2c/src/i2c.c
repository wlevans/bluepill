#include "i2c.h"

#include "FreeRTOS.h"
#include "queue.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

void i2c_wait_busy(uint32_t i2c);
void i2c_wait_start(uint32_t i2c);
void i2c_wait_address(uint32_t i2c);
void i2c_wait_transfer(uint32_t i2c);
void i2c_wait_stop(uint32_t i2c);
inline void i2c_reset(uint32_t i2c);

void i2c1_init(void)
{
  // To do:
  // Pass I2C# as parameter.
  // Add timeout feature.

  // Enable clocks.
  rcc_periph_clock_enable(RCC_I2C1);
  rcc_periph_clock_enable(RCC_GPIOB);

  // Set I2C SCL and SDA lines.
  // PB6 => SCL1
  // PB7 => SDA1
  gpio_set_mode(GPIOB,
                GPIO_MODE_OUTPUT_50_MHZ,
				GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
				GPIO6 | GPIO7);
  // Idle SCL and SDA high.
  gpio_set(GPIOB, GPIO6 | GPIO7);
  // Set baudrate to 100 KHz (standard mode).
  i2c_set_standard_mode(I2C1);
  // Set duty cycle to 50%. For standard mode duty cycle can only be 50%
  // therefore making this function call uncessary.
  i2c_set_dutycycle(I2C1, I2C_CCR_DUTY_DIV2);
  // Set the peripheral clock (PCLK) frequency to 36 MHz.
  i2c_set_clock_frequency(I2C1, 36);
  // Set the maximum rise time (TRISE).
  // TRISE = (PCLK / 1M) + 1.
  // RM00008 Section 26.6.9.
  i2c_set_trise(I2C1, 37);
  // Set the bus clock frequency (control clock)(CCR).
  // For standard mode baudrate = PCLK / (2 * CCR).
  // Therefore, CCR = PCLK / (baudrate * 2).
  // RM00008 Section 26.6.8.
  i2c_set_ccr(I2C1, 180);
  // Enable I2C peripheral.
  i2c_peripheral_enable(I2C1);
}

inline void i2c_reset(uint32_t i2c)
{
  // Reset I2C peripheral.
  I2C_CR1(i2c) |=  I2C_CR1_SWRST;
  I2C_CR1(i2c) &= ~I2C_CR1_SWRST;
  return;
}

void i2c_write(uint32_t i2c, uint8_t address, uint8_t * buffer, size_t length)
{
  // Declare local variable(s).
  uint32_t i;
  // Send data to I2C device.
  i2c_wait_busy(i2c);
  i2c_send_start(i2c);
  i2c_wait_start(i2c);
  i2c_send_7bit_address(i2c, address, I2C_WRITE);
  i2c_wait_address(i2c);
  for(i = 0; i < length; ++i)
  {
    i2c_send_data(i2c, buffer[i]);
    i2c_wait_transfer(i2c);
  }
  i2c_send_stop(i2c);
  i2c_wait_stop(i2c);
  return;
}

void i2c_read(uint32_t i2c, uint8_t address, uint8_t * buffer, size_t length)
{
  return;
}

void i2c_write_task(void *args __attribute((unused)))
{
  while(1)
  {
	  taskYIELD();
  }
  return;
}

void i2c_read_task(void *args __attribute((unused)))
{
  while(1)
  {
	  taskYIELD();
  }
  return;
}

void i2c_wait_busy(uint32_t i2c)
{
  while(I2C_SR2(i2c) & I2C_SR2_BUSY)
  {
    taskYIELD();
  }
  return;
}

void i2c_wait_start(uint32_t i2c)
{
  // Wait for start bit to be set.
  while(!(I2C_SR1(i2c) & I2C_SR1_SB))
  {
    taskYIELD();
  }
  return;
}

void i2c_wait_address(uint32_t i2c)
{
  // Wait for address bi to be set.
  while(!(I2C_SR1(i2c) & I2C_SR1_ADDR))
  {
    taskYIELD();
  }
  return;
}

void i2c_wait_transfer(uint32_t i2c)
{
  // To do:
  // I2C_SR1_BTF or I2C_SR1_TxE?

  // Wait for data to be sent.
  while(!(I2C_SR1(i2c) & I2C_SR1_BTF))
  {
    taskYIELD();
  }
  return;
}

void i2c_wait_stop(uint32_t i2c)
{
  // To do:
  // Is this function needed because we i2c_wait_busy at start of transfer?

  // Wait for stop bit to be set.
  while(!(I2C_SR2(i2c) & I2C_SR2_MSL))
  {
    taskYIELD();
  }
  return;
}
