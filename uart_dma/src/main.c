#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include "bluepill.h"
#include "uart_dma.h"

int main(void)
{
  // Initialize board.
  board_init();
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
