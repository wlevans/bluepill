#include "usart.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

struct usart_t
{
	uint32_t usart_port;
	uint32_t gpio_port;
	enum rcc_periph_clken gpio_clock;
	enum rcc_periph_clken periph_clock;
	uint32_t parity;
	uint32_t stopbits;
	uint16_t tx_pin;
	uint16_t rx_pin;
	uint16_t cts_pin;
	uint16_t rts_pin;
};

QueueHandle_t uart_txq;
QueueHandle_t uart_rxq;

int32_t usart_init(uint32_t usart, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits, bool flowcontrol)
{
	// To do:
	// Add check for already being initiated.

	// Declare local variable(s).
	struct usart_t port;

	switch(usart)
	{
		case 1:
			port.usart_port   = USART1;
			port.gpio_port    = GPIOA;
			port.gpio_clock   = RCC_GPIOA;
			port.periph_clock = RCC_USART1;
			port.tx_pin       = GPIO_USART1_TX;
			port.rx_pin       = GPIO_USART1_RX;
			port.cts_pin      = GPIO_USART1_CTS;
			port.rts_pin      = GPIO_USART1_RTS;
			break;
		case 2:
			port.usart_port   = USART2;
			port.gpio_port    = GPIOA;
			port.gpio_clock   = RCC_GPIOA;
			port.periph_clock = RCC_USART2;
			port.tx_pin       = GPIO_USART2_TX;
			port.rx_pin       = GPIO_USART2_RX;
			port.cts_pin      = GPIO_USART2_CTS;
			port.rts_pin      = GPIO_USART2_RTS;
			break;
		case 3:
			port.usart_port   = USART3;
			port.gpio_port    = GPIOB;
			port.gpio_clock   = RCC_GPIOB;
			port.periph_clock = RCC_USART3;
			port.tx_pin       = GPIO_USART3_TX;
			port.rx_pin       = GPIO_USART3_RX;
			port.cts_pin      = GPIO_USART3_CTS;
			port.rts_pin      = GPIO_USART3_RTS;
			break;
		default:
			// Invalid USART number.
			return 0;
			break;
	}

	// Test for correct number of data bits.
	if((databits != 8) && (databits != 9))
	{
		return 0;
	}

	// Test for correct parity.
	if(USART_PARITY_NONE == parity)
	{
		port.parity = USART_PARITY_NONE;
	}
	else if(USART_PARITY_EVEN == parity)
	{
		port.parity = USART_PARITY_EVEN;
	}
	else if(USART_PARITY_ODD == parity)
	{
		port.parity = USART_PARITY_ODD;
	}
	else
	{
		return 0;
	}

	// Test for stop bits.
	if(USART_STOPBITS_1 == stopbits)
	{
		port.stopbits = USART_STOPBITS_1;
	}
	else if(USART_STOPBITS_2 == stopbits)
	{
		port.stopbits = USART_STOPBITS_2;
	}
	else
	{
		return 0;
	}

	// Configure USART.
	rcc_periph_clock_enable(port.gpio_clock);
	rcc_periph_clock_enable(port.periph_clock);
	gpio_set_mode(port.gpio_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, port.tx_pin);
	gpio_set_mode(port.gpio_port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, port.rx_pin);
	usart_set_baudrate(port.usart_port, baudrate);
	usart_set_databits(port.usart_port, databits);
	usart_set_parity(port.usart_port, port.parity);
	usart_set_stopbits(port.usart_port, port.stopbits);
	usart_set_mode(port.usart_port, USART_MODE_TX_RX);

	if(flowcontrol)
	{
		gpio_set_mode(port.gpio_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, port.cts_pin);
		gpio_set_mode(port.gpio_port, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, port.rts_pin);
		usart_set_flow_control(usart, USART_FLOWCONTROL_RTS_CTS);
	}
	else
	{
		usart_set_flow_control(port.usart_port, USART_FLOWCONTROL_NONE);

	}
	// Set up UART queues.
	uart_txq = xQueueCreate(16, sizeof(uint8_t));
	uart_rxq = xQueueCreate(1, sizeof(uint8_t));

	return port.usart_port;
}

uint32_t usart_puts(uint32_t usart, const char * src)
{
	// To do: Look at making src const pointer to const string.
	uint32_t count = 0;
//	char * ptr = src;

//	for( ; *ptr; )
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

//	for(ptr = src; *ptr; ++ptr) count += usart_putc(usart, *ptr);
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

uint32_t usart_getc(uint32_t usart, char * c)
{
	// To do: Add queue to usart handler and test here.
	uint32_t result = 0;

	if(xQueueReceive(uart_rxq, c, 0) == pdPASS)
	{
		result = 1;
	}
	return result;
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
