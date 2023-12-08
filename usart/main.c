#include "FreeRTOS.h"
#include "task.h"

#include "bluepill.h"
#include "usart.h"
#include "led.h"

uint32_t usart = 0;

static void usart_tx(void *args __attribute((unused)))
{
	while(1)
	{
		usart_send(usart, 'w');
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	return;
}

static void usart_rx(void *args __attribute((unused)))
{
	static uint8_t data = 0;
	while(1)
	{
		if(!(USART_SR(usart) & USART_SR_TXE))
		{
			led_toggle();
			data = usart_recv(usart);
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	return;
}

int main(void)
{
	// Set up board.
	board_init();
	// Set up LED.
	led_init();

	// Set up USART 1
	usart = usart_init(1, 38400, 8, USART_PARITY_NONE, USART_STOPBITS_1, false);

	// Create FreeRTOS tasks.
	xTaskCreate(usart_tx, "usart_tx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(usart_rx, "usart_rx", 100, NULL, configMAX_PRIORITIES - 1, NULL);

	// Start FreeRTOS scheduler.
	vTaskStartScheduler();

	// Will only get here if the scheduler fails to start.
	while(1);

	return 0;
}
