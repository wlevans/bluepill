#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#include "led.h"
#include "uart_dma.h"

// To do: Should event group go here or in uart_dma.*?
EventGroupHandle_t uart_dma_eventgroup;

int main(void)
{
	// Set up clock.
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);
	// Initialize LED.
	led_init();
	// Initialize USART 1.
	uart1_init();
	// Initialize DMA 1.
	dma1_init();

	uart_dma_eventgroup = xEventGroupCreate();

	// Create FreeRTOS tasks.
	xTaskCreate(usart_rx, "usart_rx", 256, NULL, configMAX_PRIORITIES - 1, NULL);
	xTaskCreate(usart_tx, "usart_tx", 256, NULL, configMAX_PRIORITIES - 1, NULL);

	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
	// Will only get here if the scheduler fails to start.
	while(1);
	return 0;
}
