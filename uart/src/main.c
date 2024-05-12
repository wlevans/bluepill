#include <stddef.h>

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include <libopencm3/stm32/rcc.h>
#include "uart.h"

static QueueHandle_t uart_q;

static void uart_tx(void *args __attribute((unused)))
{
  static uint8_t data = 0;

  while(1)
  {
    if(xQueueReceive(uart_q, &data, 500) == pdPASS)
    {
      while(!usart_get_flag(USART1, USART_SR_TXE))
      {
        taskYIELD();
      }
      usart_send(USART1, data);
    }
  }
  return;
}

static void uart_rx(void *args __attribute((unused)))
{
  static uint8_t data = 0;

  while(1)
  {
    if(usart_get_flag(USART1, USART_SR_RXNE))
    {
      data = usart_recv(USART1);
      while(xQueueSend(uart_q, &data, 0) != pdPASS);
    }
    taskYIELD();
  }
  return;
}

int main(void)
{
  // Set up clock.
  rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
  // Set up UART 1
  uart1_init();
  // Enable USART.
  usart_enable(USART1);
  // Set up UART queue.
  uart_q = xQueueCreate(1, sizeof(uint8_t));
  // Create FreeRTOS tasks.
  xTaskCreate(uart_tx, "uart_tx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(uart_rx, "uart_rx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
  // Start FreeRTOS scheduler.
  vTaskStartScheduler();
  // Will only get here if the scheduler fails to start.
  while(1);
  return 0;
}
