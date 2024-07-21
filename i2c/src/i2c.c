#include "i2c.h"

#include "FreeRTOS.h"
#include "queue.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <libopencm3/stm32/gpio.h>

typedef struct
{
  uint32_t port;
  uint32_t dutycycle;
  uint16_t scl_pin;
  uint16_t sda_pin;
  uint16_t ccr;
  uint16_t trise;
} i2c_parameters_t;

void i2c_wait_busy(uint32_t i2c);
void i2c_wait_start(uint32_t i2c);
void i2c_wait_address(uint32_t i2c);
void i2c_wait_transfer(uint32_t i2c);
void i2c_wait_stop(uint32_t i2c);
inline void i2c_reset(uint32_t i2c);

i2c_error_t i2c1_init(uint32_t i2c_port, i2c_mode_t i2c_mode)
{
  // To do:
  // Add timeout feature.

  /*
   * RM00008 Section 26.6.8.
   * I2C bus clock frequency (control clock) (CCR)
   *
   * For standard mode                   : CCR = Fpclk / ( 2 * baudrate) = 180.
   * For fast mode with 33.3% duty cycle : CCR = Fpclk / ( 3 * baudrate) =  30.
   * For fast mode with 36% duty cycle   : CCR = Fpclk / (25 * baudrate) =   3.6.
   *
   * Note: Do not use a 36% duty cycle since it yeilds a fraction for CCR.
   *
   * RM00008 Section 26.6.9.
   * I2C rise time (Trise) must be programmed with the maximum SCL rise time
   * given by the I2C bus specification.
   *
   * For 100 KHz, the maximum rise time is : 1000 ns.
   * For 400 KHz, the maximum rise time is : 300 ns.
   *
   * The rise time is calculated as:
   *
   *   Trise = (Trise_max * Fpclk) + 1
   *
   * For standard mode (100 MHz) : Trise = 37.
   * For fast mode (400 MHz)     : Trise = 11.
   */

  // Check I2C port number.
  if(!(i2c_port < I2C_PORT_COUNT))
  {
    return(I2C_ERROR_PORT);
  }
  // Test I2C mode.
  if((MODE_STANDARD != i2c_mode) && (MODE_FAST != i2c_mode))
  {
    return(I2C_ERROR_MODE);
  }

  // Declare local variable(s).
  i2c_parameters_t i2c_parameters = {0};

  // Determine port parameters.
  switch(i2c_port)
  {
    case I2C_PORT_1:
      i2c_parameters.port = I2C1;
      i2c_parameters.scl_pin = GPIO6;
      i2c_parameters.sda_pin = GPIO7;
      break;
    case I2C_PORT_2:
      i2c_parameters.port = I2C2;
      i2c_parameters.scl_pin = GPIO10;
      i2c_parameters.sda_pin = GPIO11;
      break;
  }
  // Determine mode parameters.
  switch(i2c_mode)
  {
    case MODE_STANDARD:
      // Set baudrate to 100 KHz.
      i2c_set_standard_mode(i2c_parameters.port);
      i2c_parameters.dutycycle = I2C_CCR_DUTY_DIV2;
      i2c_parameters.ccr = 180;
      i2c_parameters.trise = 37;
      break;
    case MODE_FAST:
      // Set baudrate to 400 KHz.
      i2c_set_fast_mode(i2c_parameters.port);
      i2c_parameters.dutycycle = I2C_CCR_DUTY_DIV2;
      i2c_parameters.ccr = 30;
      i2c_parameters.trise = 11;
      break;
  }
  // Enable clocks.
  rcc_periph_clock_enable(RCC_I2C1);
  rcc_periph_clock_enable(RCC_GPIOB);
  // For the blue pill, the I2C peripheral clock frequency (Fpclk) = 36 MHz.
  i2c_set_clock_frequency(i2c_parameters.port, 36);
  // Set SCL and SDA lines.
  gpio_set_mode(GPIOB,
                GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
				i2c_parameters.scl_pin | i2c_parameters.sda_pin );
  // Idle SCL and SDA high.
  gpio_set(GPIOB, i2c_parameters.scl_pin | i2c_parameters.sda_pin );
  // Configure port.
  i2c_set_ccr(i2c_parameters.port, i2c_parameters.ccr);
  i2c_set_trise(i2c_parameters.port, i2c_parameters.trise);
  i2c_set_dutycycle(i2c_parameters.port, i2c_parameters.dutycycle);
  // Enable I2C peripheral.
  i2c_peripheral_enable(I2C1);

  return(I2C_ERROR_OK);
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
//    taskYIELD();
  }
  return;
}

void i2c_wait_start(uint32_t i2c)
{
  // Wait for start bit to be set.
  while(!(I2C_SR1(i2c) & I2C_SR1_SB))
  {
//    taskYIELD();
  }
  return;
}

void i2c_wait_address(uint32_t i2c)
{
  // Wait for address bi to be set.
  while(!(I2C_SR1(i2c) & I2C_SR1_ADDR))
  {
//    taskYIELD();
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
//    taskYIELD();
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
//    taskYIELD();
  }
  return;
}
