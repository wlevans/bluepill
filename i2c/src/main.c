#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <libopencm3/stm32/rcc.h>
#include "led.h"
#include "i2c.h"
#include "display.h"

void blink_task(void *args __attribute((unused)));
// To do:
// Display module.

int main(void)
{
  // Set up clock.
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  // Set up LED.
  led_init();
  // Set up I2C 1.
  i2c_handle_t i2c_handle = i2c1_init(I2C_PORT_1, I2C_MODE_STANDARD);
  // Set up display.
  display_handle_t display_handle = display_init(i2c_handle, 0x3C);

//{
//  // Set up display.
//  display_init();
//  command(0x01);
//  command(0x02);
//  command(0xa0);
//  data(0x93);
//  data(0x20);
//  data(0x48);
//  data(0x65);
//  data(0x6C);
//  data(0x6C);
//  data(0x6F);
//  data(0x20);
//  data(0x57);
//  data(0x69);
//  data(0x6C);
//  data(0x6C);
//  data(0x79);
//  data(0x20);
//  data(0x93);
//}

  // Create FreeRTOS tasks.
  xTaskCreate(blink_task, "blink_task", 100, NULL, configMAX_PRIORITIES - 1, NULL);
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
