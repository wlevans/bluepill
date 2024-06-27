#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <libopencm3/stm32/rcc.h>
#include "i2c.h"

int main(void)
{
  // Set up clock.
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  // Set up I2C 1.
  i2c1_init();
  // Create FreeRTOS tasks.
  xTaskCreate(i2c_write_task, "i2c_write_task", 100, NULL, configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(i2c_read_task, "i2c_read_task", 100, NULL, configMAX_PRIORITIES - 1, NULL);
  // Start FreeRTOS scheduler.
  vTaskStartScheduler();
  // Will only get here if the scheduler fails to start.
  while(1);
  return 0;
}
