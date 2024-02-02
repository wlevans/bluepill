#include "uart.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

void uart1_init(void)
{
	// Enable clocks.
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	// Configure USART 1.
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	usart_set_baudrate(USART1, 38400);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	// Enable USART RX interrupt.
	usart_enable_rx_interrupt(USART1);
	// Set USART 1 interrupt priority.
	nvic_set_priority(NVIC_USART1_IRQ, 0xCF);
	// Enable USART 1 interrupt.
	nvic_enable_irq(NVIC_USART1_IRQ);
	// Enable USART.
	usart_enable(USART1);

	return;
}

void usart1_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return;
}
