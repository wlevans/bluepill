#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/cm3/nvic.h>

#include "uart_dma.h"
#include "led.h"

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

	// Enable USART RX interrupt.
	usart_enable_rx_interrupt(USART1);
	// Set USART 1 interrupt priority.
	nvic_set_priority(NVIC_USART1_IRQ, 0xCF);
	// Enable USART 1 interrupt.
	nvic_enable_irq(NVIC_USART1_IRQ);

	// Enable USART.
	usart_enable(USART1);

	// Create FreeRTOS tasks.
	// To do: create tasks.

	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
	// Will only get here if the scheduler fails to start.
	while(1);
	return 0;
}
