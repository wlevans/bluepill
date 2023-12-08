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
	static bool result = false;
	while(1)
	{
		result = usart_get_flag(usart, USART_SR_RXNE);
		if(result)
		{
			usart_recv(usart);
			led_toggle();
		}
		vTaskDelay(pdMS_TO_TICKS(10));
	}
	return;
}

int main(void)
{
	// Initialize board.
	board_init();
	// Initialzie LED.
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
