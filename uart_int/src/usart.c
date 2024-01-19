#include "usart.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

QueueHandle_t uart_txq;
QueueHandle_t uart_rxq;

void usart1_init(void)
{
	// Configure USART 1.
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	usart_set_baudrate(USART1, 38400);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	// Set up UART queues.
	uart_txq = xQueueCreate(16, sizeof(uint8_t));
	uart_rxq = xQueueCreate(1, sizeof(uint8_t));
	return;
}

uint32_t usart_puts(uint32_t usart, const char * src)
{
	uint32_t count = 0;

	while(*src)
	{
		if(xQueueSend(uart_txq, src++, pdMS_TO_TICKS(10)) == pdPASS)
		{
			++count;
			usart_enable_tx_interrupt(usart);
		}
		else
		{
			break;
		}
	}

	return count;
}

uint32_t usart_putc(uint32_t usart, char c)
{
	if(xQueueSend(uart_txq, &c, pdMS_TO_TICKS(10)) == pdPASS)
	{
		usart_enable_tx_interrupt(usart);
		return 1;
	}
	return 0;
}

uint32_t usart_getc(char * c)
{
	if(xQueueReceive(uart_rxq, c, 0) == pdPASS)
	{
		return 1;
	}
	return 0;
}

void usart1_isr(void)
{
	uint8_t data = 0;
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Test for TX interrupt.
	if(usart_get_flag(USART1, USART_SR_TXE))
	{
		if(xQueueReceiveFromISR(uart_txq, &data, &xHigherPriorityTaskWoken))
		{
			usart_send(USART1, data);
		}
		else
		{
			usart_disable_tx_interrupt(USART1);
		}
	}

	// Test for RX interrupt.
	if(usart_get_flag(USART1, USART_SR_RXNE))
	{
		data = usart_recv(USART1);
		xQueueSendFromISR(uart_rxq, &data, &xHigherPriorityTaskWoken);
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return;
}
