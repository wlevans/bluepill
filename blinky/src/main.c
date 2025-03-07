#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include "bluepill.h"
#include "led.h"

static void blink(void *args __attribute((unused)))
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

int main(void)
{
  // Initialize board.
  board_init();
  // Initialize LED.
  led_init();

  // Create FreeRTOS task.
  xTaskCreate(blink, "blink", 100, NULL, configMAX_PRIORITIES - 1, NULL);
  // Start FreeRTOS scheduler.
  vTaskStartScheduler();

  // Will only get here if the scheduler fails to start.
  while(1);

  return 0;
}
