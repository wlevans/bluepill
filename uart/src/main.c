#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bluepill.h"
#include "uart.h"

uint32_t uart = 0;
static QueueHandle_t uart_q;

static void uart_tx(void *args __attribute((unused)))
{
	static uint8_t data = 0;

	while(1)
	{
		if(xQueueReceive(uart_q, &data, 500) == pdPASS)
		{
			while(!usart_get_flag(uart, USART_SR_TXE))
			{
				taskYIELD();
			}
			usart_send(uart, data);
		}
	}
	return;
}

static void uart_rx(void *args __attribute((unused)))
{
	static uint8_t data = 0;

	while(1)
	{
		if(usart_get_flag(uart, USART_SR_RXNE))
		{
			data = usart_recv(uart);
			xQueueSend(uart_q, &data, portMAX_DELAY);
		}
		taskYIELD();
	}
	return;
}

int main(void)
{
	// Initialize bluepill.
	bluepill_init();
	// Set up UART 1
	uart = usart_init(1, 38400, 8, USART_PARITY_NONE, USART_STOPBITS_1, false);
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
