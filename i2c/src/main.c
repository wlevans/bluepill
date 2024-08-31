#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <libopencm3/stm32/rcc.h>
#include "led.h"
#include "i2c.h"
#include "display.h"

#define DISPLAY_ADDRESS 0x3C

void blink_task(void *args __attribute((unused)));
void display_task(void * args);

int main(void)
{
  // Declare local variables.
  static i2c_interface_t i2c_interface;

  // Set up clock.
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  // Set up LED.
  led_init();
  // Set up I2C 1.
  i2c1_init(&i2c_interface, I2C_PORT_1, I2C_MODE_STANDARD);
  // Set up display.
  display_init(&i2c_interface, DISPLAY_ADDRESS);

  // Create FreeRTOS tasks.
  xTaskCreate(blink_task, "blink_task", 100, NULL, 0, NULL);
  xTaskCreate(display_task, "display_task", 100, &i2c_interface, 0, NULL);
  // Start FreeRTOS scheduler.
  vTaskStartScheduler();
  // Will only get here if the scheduler fails to start.
  while(1);
  return 0;
}

void blink_task(void *args __attribute((unused)))
{
  while(1)
  {
    led_toggle();
    if(led_get())
    {
      // If LED is on.
      vTaskDelay(pdMS_TO_TICKS(250));
    }
    else
    {
      // If LED is off.
      vTaskDelay(pdMS_TO_TICKS(750));
    }
  }
  return;
}

void display_task(void * args)
{
  // Get task arguments.
  i2c_interface_t * i2c_interface = (i2c_interface_t *)args;
  size_t i = 0;

  uint8_t setup_command[] = {COMMAND, 0x01, 0x02, 0x80};
  uint8_t setup_data[] = {DATA, 0x93, 0x20, 0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x69, 0x6C, 0x6C, 0x79, 0x20, 0x93};

  while(1)
  {
    {
      for(i = 0; i < 4; ++i)
      {
        setup_command[3] = (0x80 | (i << 5)) + i;
        i2c_interface->write(i2c_interface->port, DISPLAY_ADDRESS, setup_command, sizeof(setup_command));
        i2c_interface->write(i2c_interface->port, DISPLAY_ADDRESS, setup_data, sizeof(setup_data));
        vTaskDelay(pdMS_TO_TICKS(500));
      }
    }
  }
}
