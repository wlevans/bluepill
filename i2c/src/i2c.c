#include "i2c.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

void i2c1_init(void)
{
  // Enable clocks.
  rcc_periph_clock_enable(RCC_I2C1);
  rcc_periph_clock_enable(GPIOB);

  // Set I2C SCL and SDA lines.
  gpio_set_mode(GPIOB,
                GPIO_MODE_OUTPUT_50_MHZ,
				GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
				GPIO6|GPIO7);
  // Idle SCL and SDA high.
  gpio_set(GPIOB, GPIO6|GPIO7);
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

/*
 * To do:
 *
 * Find defautl register values:
 *   Reset
 *   Enable
 *   Start
 *   Stop
 *   Duty cycle
 *
 * Should i2c1_init(void) be i2c_init(uint32_t i2c)?
 *
 * // Disable I2C peripheral.
 * i2c_peripheral_disable(I2C1);
 * // Reset I2C peripheral.
 * i2c_reset(I2C1);
 * // Clear stop.
 * i2c_clear_stop(I2C1);
 */
}

inline void i2c_reset(uint32_t i2c)
{
  // Reset I2C peripheral.
  I2C_CR1(i2c) |=  I2C_CR1_SWRST;
  I2C_CR1(i2c) &= ~I2C_CR1_SWRST;
  return;
}

void i2c_write(uint32_t i2c, uint8_t * buffer, size_t length)
{
  return;
}

void i2c_read(uint32_t i2c, uint8_t * buffer, size_t length)
{
  return;
}

void i2c_write_task(void *args __attribute((unused)))
{
  while(1)
  {
  }
  return;
}

void i2c_read_task(void *args __attribute((unused)))
{
  while(1)
  {
  }
  return;
}
