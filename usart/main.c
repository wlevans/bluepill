#include "FreeRTOS.h"
#include "task.h"

#include "bluepill.h"
#include "usart.h"

static void usart_tx(void *args __attribute((unused)))
{
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	return;
}

static void usart_rx(void *args __attribute((unused)))
{
	while(1)
	{
		vTaskDelay(pdMS_TO_TICKS(100));
	}
	return;
}

int main(void)
{
	// Set up board.
	board_init();

	// Set up USART 1
	usart_init(1, 38000, 8, USART_PARITY_NONE, USART_STOPBITS_1, false);

	// Create FreeRTOS tasks.
	xTaskCreate(usart_tx, "usart_tx", 100, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(usart_rx, "usart_rx", 100, NULL, configMAX_PRIORITIES - 1, NULL);

	// Start FreeRTOS scheduler.
	vTaskStartScheduler();

	// Will only get here if the scheduler fails to start.
	while(1);

	return 0;
}
