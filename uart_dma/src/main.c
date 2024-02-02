#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#include "led.h"
#include "uart.h"
#include "dma.h"

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


	// Create FreeRTOS tasks.
	// To do: create tasks.

	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
	// Will only get here if the scheduler fails to start.
	while(1);
	return 0;
}
