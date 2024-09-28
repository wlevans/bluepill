#include "i2c.h"

#include <stdlib.h>

#include "FreeRTOS.h"
#include "semphr.h"

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
void i2c_wait_receive(uint32_t i2c);
void i2c_wait_stop(uint32_t i2c);
void i2c_reset(uint32_t i2c);
void i2c_clear_ack_failure(uint32_t i2c);

i2c_error_t i2c1_init(i2c_interface_t * i2c_interface, uint32_t i2c_port, i2c_mode_t i2c_mode)
{
  // To do:
  // Add timeout feature.

  /*
   * RM00008 Section 26.6.8.
   * I2C bus clock frequency (control clock) (CCR)
   *
   * For standard mode                   : CCR = Fpclk / ( 2 * baud rate) = 180.
   * For fast mode with 33.3% duty cycle : CCR = Fpclk / ( 3 * baud rate) =  30.
   * For fast mode with 36% duty cycle   : CCR = Fpclk / (25 * baud rate) = 3.6.
   *
   * Note: Do not use a 36% duty cycle since it yields a fraction for CCR.
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

  // Check I2C structure is not NULL.
  if(NULL == i2c_interface)
  {
    return(I2C_ERROR_INVALID);
  }
  // Check I2C port number.
  if(!(i2c_port < I2C_PORT_COUNT))
  {
    return(I2C_ERROR_PORT);
  }
  // Test I2C mode.
  if(!(i2c_port < I2C_MODE_COUNT))
  {
    return(I2C_ERROR_MODE);
  }

  // Declare local variable(s).
  i2c_parameters_t i2c_parameters = {0};

  // Determine port parameters.
  switch(i2c_port)
  {
    case I2C_PORT_1:
      i2c_interface->port = I2C1;
      i2c_parameters.scl_pin = GPIO6;
      i2c_parameters.sda_pin = GPIO7;
      break;
    case I2C_PORT_2:
      i2c_interface->port = I2C2;
      i2c_parameters.scl_pin = GPIO10;
      i2c_parameters.sda_pin = GPIO11;
      break;
  }
  // Determine mode parameters.
  switch(i2c_mode)
  {
    case I2C_MODE_STANDARD:
      // Set baud rate to 100 KHz.
      i2c_set_standard_mode(i2c_interface->port);
      i2c_parameters.dutycycle = I2C_CCR_DUTY_DIV2;
      i2c_parameters.ccr = 180;
      i2c_parameters.trise = 37;
      break;
    case I2C_MODE_FAST:
      // Set baud rate to 400 KHz.
      i2c_set_fast_mode(i2c_interface->port);
      i2c_parameters.dutycycle = I2C_CCR_DUTY_DIV2;
      i2c_parameters.ccr = 30;
      i2c_parameters.trise = 11;
      break;
  }
  // Point to I2C functions.
  i2c_interface->read = i2c_read;
  i2c_interface->write = i2c_write;

  // Enable clocks.
  rcc_periph_clock_enable(RCC_I2C1);
  rcc_periph_clock_enable(RCC_GPIOB);
  // For the blue pill, the I2C peripheral clock frequency (Fpclk) = 36 MHz.
  i2c_set_clock_frequency(i2c_interface->port, 36);
  // Set SCL and SDA lines.
  gpio_set_mode(GPIOB,
                GPIO_MODE_OUTPUT_50_MHZ,
                GPIO_CNF_OUTPUT_ALTFN_OPENDRAIN,
                i2c_parameters.scl_pin | i2c_parameters.sda_pin );
  // Idle SCL and SDA high.
  gpio_set(GPIOB, i2c_parameters.scl_pin | i2c_parameters.sda_pin );
  // Configure port.
  i2c_set_ccr(i2c_interface->port, i2c_parameters.ccr);
  i2c_set_trise(i2c_interface->port, i2c_parameters.trise);
  i2c_set_dutycycle(i2c_interface->port, i2c_parameters.dutycycle);
  // Enable I2C peripheral.
  i2c_peripheral_enable(i2c_interface->port);

  return(I2C_ERROR_OK);
}

void i2c_write(uint32_t port, uint8_t address, uint8_t * buffer, size_t length)
{
  // Declare local variable(s).
  uint32_t i;

  // Send data to I2C device.
  i2c_wait_busy(port);
  // To do: must ACK failure be cleared?
  i2c_clear_ack_failure(port);
  i2c_send_start(port);
  i2c_wait_start(port);
  i2c_send_7bit_address(port, address, I2C_WRITE);
  i2c_wait_address(port);
  for(i = 0; i < length; ++i)
  {
    i2c_send_data(port, *(buffer + i));
    i2c_wait_transfer(port);
  }
  i2c_send_stop(port);
  i2c_wait_stop(port);
  return;
}

void i2c_read(uint32_t port, uint8_t address, uint8_t * buffer, size_t length)
{
  // Declare local variable(s).
  uint32_t i;

  // Receive data from I2C device.
  i2c_wait_busy(port);
  // To do: must ACK failure be cleared?
  i2c_clear_ack_failure(port);
  i2c_enable_ack(port);
  i2c_send_start(port);
  i2c_wait_start(port);
  i2c_send_7bit_address(port, address, I2C_READ);
  i2c_wait_address(port);
  for(i = 0; i < length; ++i)
  {
    // To do: Debug why only first byte is received.
    if(i == length)
    {
      i2c_disable_ack(port);
    }
    i2c_wait_receive(port);
    *(buffer + i) = i2c_get_data(port);
  }
  i2c_send_stop(port);
  i2c_wait_stop(port);
  return;
}

void i2c_write_read(uint32_t port, uint8_t address, uint8_t * write_buffer, size_t write_length, uint8_t * read_buffer, size_t read_length)
{
  // Declare local variable(s).
  uint32_t i;

  // Send data to I2C device.
  i2c_wait_busy(port);
  // To do: must ACK failure be cleared?
  i2c_clear_ack_failure(port);
  i2c_send_start(port);
  i2c_wait_start(port);
  i2c_send_7bit_address(port, address, I2C_WRITE);
  i2c_wait_address(port);
  for(i = 0; i < write_length; ++i)
  {
    i2c_send_data(port, *(write_buffer + i));
    if(i == write_length - 1)
    {
      // To do: Do we need taskENTER_CRITICAL();
      i2c_send_start(port);
    }
    i2c_wait_transfer(port);
  }
  i2c_enable_ack(port);
  i2c_wait_start(port);
  i2c_send_7bit_address(port, address, I2C_READ);
  i2c_wait_address(port);
  for(i = 0; i < read_length; ++i)
  {
    // To do: Debug why only first byte is received.
    if(i == read_length)
    {
      i2c_disable_ack(port);
    }
    i2c_wait_receive(port);
    *(read_buffer + i) = i2c_get_data(port);
  }
  i2c_send_stop(port);
  i2c_wait_stop(port);
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

void i2c_wait_receive(uint32_t i2c)
{
  // Wait for data to be received.
  while(!(I2C_SR1(i2c) & I2C_SR1_RxNE))
  {
    taskYIELD();
  }
  return;
}

void i2c_wait_stop(uint32_t i2c)
{
  // Wait for master/slave stop bit to be clear (slave mode).
  while(!(I2C_SR2(i2c) & I2C_SR2_MSL))
  {
    taskYIELD();
  }
  return;
}

void i2c_reset(uint32_t i2c)
{
  // Reset I2C peripheral.
  I2C_CR1(i2c) |=  I2C_CR1_SWRST;
  I2C_CR1(i2c) &= ~I2C_CR1_SWRST;
  return;
}

void i2c_clear_ack_failure(uint32_t i2c)
{
  I2C_SR1(i2c) &= ~I2C_SR1_AF;
  return;
}
