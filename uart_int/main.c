#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bluepill.h"
#include "usart.h"
#include "led.h"

#include <libopencm3/cm3/nvic.h>

uint32_t uart = 0;
static QueueHandle_t uart_txq;
static QueueHandle_t uart_rxq;
static char *menu[] = {"\x1B[2J\x1B[H1. LED On\n\r",
		"2. LED Off\n\r",
		"3. LED Toggle\n\n\r",
		"LED state: "}; //("\033[5;11H")

void usart1_isr(void)
{
	uint8_t data = 0;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Test for RX interrupt.
	if(usart_get_flag(uart, USART_SR_RXNE))
	{
		data = usart_recv(uart);
		xQueueSendFromISR(uart_rxq, &data, &xHigherPriorityTaskWoken);
	}

	// Test for TX interrupt.
	if(usart_get_flag(uart, USART_SR_TXE))
	{
		if(xQueueReceiveFromISR(uart_txq, &data, &xHigherPriorityTaskWoken))
		{
			usart_send(uart, data);
		}
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return;
}

static void process_cmd(void *args __attribute((unused)))
{
//	static uint8_t data = 0;

	// Transmit menu.
	for(uint8_t i = 0; i < sizeof(menu) / sizeof(menu[0]); ++i)
	{
		for(char * s = menu[i]; *s; ++s)
		{
			while(!usart_get_flag(uart, USART_SR_TXE))
			{
				taskYIELD();
			}
			usart_send(uart, *s);
		}
	}
	// Send LED state. 1 for on; 0 for off.
	while(!usart_get_flag(uart, USART_SR_TXE))
	{
		taskYIELD();
	}
	usart_send(uart, led_get()?'1':'0');

	while(1)
	{
		taskYIELD();
	}
	return;
}

int main(void)
{
	// Initialize board.
	board_init();
	// Initialize LED.
	led_init();
//	led_toggle();

	// Set up USART.
	uart = usart_init(1, 38400, 8, USART_PARITY_NONE, USART_STOPBITS_1, false);
	// Enable USART RX and TX interrupts.
	usart_enable_rx_interrupt(uart);
	usart_enable_tx_interrupt(uart);
	// Enable USART.
	usart_enable(uart);
	// Set up UART queue.
	uart_txq = xQueueCreate(16, sizeof(uint8_t));
	uart_rxq = xQueueCreate(1, sizeof(uint8_t));
	// Create FreeRTOS tasks.
	xTaskCreate(process_cmd, "uart_tx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
	// Will only get here if the scheduler fails to start.
	while(1);
	return 0;
}
