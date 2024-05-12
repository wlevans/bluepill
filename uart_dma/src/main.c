#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#include "uart_dma.h"

int main(void)
{
  // Set up system clock.
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  // Initialize USART 1 and DMA.
  uart1_init();

  // Create FreeRTOS tasks.
  xTaskCreate(usart_rx_task, "usart_rx_task", 256, NULL, configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(usart_tx_task, "usart_rx_task", 256, NULL, configMAX_PRIORITIES - 1, NULL);

  // Start FreeRTOS scheduler.
  vTaskStartScheduler();
  // Will only get here if the scheduler fails to start.
  while(1);
}
