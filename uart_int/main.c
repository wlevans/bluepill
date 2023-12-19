#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "bluepill.h"
#include "usart.h"
#include "led.h"

uint32_t uart = 0;
static QueueHandle_t uart_txq;
static QueueHandle_t uart_rxq;
static char *menu[] = {"\x1B[2J\x1B[H1. LED On\n\r","2. LED Off\n\r","3. LED Toggle\n\n\r","LED state: "};

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

static void uart_tx(void *args __attribute((unused)))
{
//	static uint8_t data = 0;

	// Transmit menu.
	for(uint8_t i = 0; i < sizeof(menu) / sizeof(menu[0]); ++i)
	{
		char * s = menu[i];
		for( ; *s; ++s)
		{
			while(!usart_get_flag(uart, USART_SR_TXE))
			{
				taskYIELD();
			}
			usart_send(uart, *s);
		}
	}

	while(1)
	{
		taskYIELD();
	}
	return;
}

static void uart_rx(void *args __attribute((unused)))
{
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
	// Set up USART.
	uart = usart_init(1, 38400, 8, USART_PARITY_NONE, USART_STOPBITS_1, false);
	// Set up USART interrupts.
	/*  To do: Configure USART interrupts. */
	// Enable USART.
	usart_enable(uart);
	// Set up UART queue.
	uart_txq = xQueueCreate(16, sizeof(uint8_t));
	uart_rxq = xQueueCreate(1, sizeof(uint8_t));
	// Create FreeRTOS tasks.
	xTaskCreate(uart_tx, "uart_tx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(uart_rx, "uart_rx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	// Create FreeRTOS task.
	xTaskCreate(blink, "blink", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
	// Will only get here if the scheduler fails to start.
	while(1);
	return 0;
}
