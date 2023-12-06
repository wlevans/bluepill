#include "FreeRTOS.h"
#include "task.h"

#include "bluepill.h"
#include "usart.h"

static void blink(void *args __attribute((unused)))
{
	while(1)

	return;
}

int main(void)
{
	// Set up board.
	board_init();

	// Set up USART 1
	usart_init(1, 38000, 8, USART_PARITY_NONE, 2, false);

	// Create FreeRTOS task.
	xTaskCreate(blink, "blink", 100, NULL, configMAX_PRIORITIES - 1, NULL);

	// Start FreeRTOS scheduler.
	vTaskStartScheduler();

	// Will only get here if the scheduler fails to start.
	while(1);

	return 0;
}
