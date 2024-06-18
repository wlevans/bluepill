#include "i2c.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

void i2c_init(void)
{
  // Enable clock.
  rcc_periph_clock_enable(RCC_I2C1);

  // Configure I2C 1.
  gpio_set_mode(GPIOB,
                GPIO_MODE_OUTPUT_50_MHZ,
				GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
				GPIO6|GPIO7);
  // Idle SCL and SDA high.
  gpio_set(GPIOB, GPIO6|GPIO7);
  // Disable I2C peripheral.
  i2c_peripheral_disable(I2C1);
  // Reset I2C peripheral.
  I2C_CR1(I2C1) |=  I2C_CR1_SWRST;
  I2C_CR1(I2C1) &= ~I2C_CR1_SWRST;
  // Clear stop.
  I2C_CR1(I2C1) &= ~I2C_CR1_STOP;
  // Set standard (100 kHz) mode.
  i2c_set_standard_mode(I2C1);
  // Set duty cycle to 50%.
  i2c_set_dutycycle(I2C1, I2C_CCR_DUTY_DIV2);
  // Set the peripheral clock frequency.
  i2c_set_clock_frequency(I2C1, 36);
  // Set the maximum rise time.
  // To do: How is this determined?
  i2c_set_trise(I2C1, 36);
  // Set the bus clock frequency.
  // To do: How is this determined?
  i2c_set_ccr(I2C1, 180);
  // Enable I2C peripheral.
  i2c_peripheral_enable(I2C1);
}
