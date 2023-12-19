#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bluepill.h"
#include "usart.h"

uint32_t uart = 0;
static QueueHandle_t uart_tx_q;
static QueueHandle_t uart_rx_q;

static void uart_tx(void *args __attribute((unused)))
{
	while(1)
	{

	}
	return;
}

static void uart_rx(void *args __attribute((unused)))
{
	while(1)
	{

	}
	return;
}

int main(void)
{
	// Initialize board.
	board_init();
	// Set up USART.
	uart = usart_init(1, 38400, 8, USART_PARITY_NONE, USART_STOPBITS_1, false);
	// Set up USART interrupts.
	/*  To do: Configure USART interrupts. */
	// Enable USART.
	usart_enable(uart);
	// Set up UART queue.
	uart_tx_q = xQueueCreate(16, sizeof(uint8_t));
	uart_rx_q = xQueueCreate(1, sizeof(uint8_t));
	// Create FreeRTOS tasks.
	xTaskCreate(uart_tx, "uart_tx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(uart_rx, "uart_rx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
	// Will only get here if the scheduler fails to start.
	while(1);
	return 0;
}
