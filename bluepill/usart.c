#include "usart.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

struct usart_t
{
	uint32_t usart;  // USART number.
};

usart_handle_t usart_init(uint32_t usart, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits, bool flowcontrol)
{
	// Declare local variable(s).
	uint32_t gpio_port;
	enum rcc_periph_clken gpio_clock;
	enum rcc_periph_clken usart_clock;
	uint16_t usart_tx;
	uint16_t usart_rx;
	uint16_t usart_cts;
	uint16_t usart_rts;
	uint32_t usart_stopbits;
	switch(usart)
	{
		case 1:
			gpio_port   = GPIOA;
			gpio_clock  = RCC_GPIOA;
			usart_clock = RCC_USART1;
			usart_tx    = GPIO_USART1_TX;
			usart_rx    = GPIO_USART1_RX;
			usart_cts   = GPIO_USART1_TX;
			usart_rts   = GPIO_USART1_TX;
			break;
		case 2:
			gpio_port   = GPIOA;
			gpio_clock  = RCC_GPIOA;
			usart_clock = RCC_USART2;
			usart_tx    = GPIO_USART2_TX;
			usart_rx    = GPIO_USART2_RX;
			usart_cts   = GPIO_USART2_TX;
			usart_rts   = GPIO_USART2_TX;
			break;
		case 3:
			gpio_port   = GPIOB;
			gpio_clock  = RCC_GPIOB;
			usart_clock = RCC_USART3;
			usart_tx    = GPIO_USART3_TX;
			usart_rx    = GPIO_USART3_RX;
			usart_cts   = GPIO_USART3_TX;
			usart_rts   = GPIO_USART3_TX;
			break;
		default:
			// Invalid USART number.
			return NULL;
			break;
	}

	// Test for correct number of databits.
	if((databits != 8) && (databits != 9))
	{
		return NULL:
	}

	// Test for correct parity.
	// To do.

	// Test for stop bits.
	if(1 == stopbits)
	{
		usart_stopbits = USART_STOPBITS_1;
	}
	else if(2 == stopbits)
	{
		usart_stopbits = USART_STOPBITS_2;
	}
	else
	{
		return NULL;
	}

	// Configure USART.
	rcc_periph_clock_enable(gpio_clock);
	rcc_periph_clock_enable(usart_clock);
	gpio_set_mode(gpio_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, usart_tx);
	gpio_set_mode(gpio_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_MODE_INPUT_PULL_UPDOWN, usart_rx);
	usart_set_baudrate(usart, baudrate);
	usart_set_databits(usart, databits);
	usart_set_stopbits(usart, stopbits);
	usart_set_mode(usart, USART_MODE_TX_RX);
	usart_set_stopbits(usart, usart_stopbits);
	if(flowcontrol)
	{
		gpio_set_mode(gpio_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, usart_cts);
		gpio_set_mode(gpio_port, GPIO_MODE_OUTPUT_50_MHZ, GPIO_MODE_INPUT_PULL_UPDOWN, usart_rts);
		usart_set_flow_control(usart, USART_FLOWCONTROL_RTS_CTS);
	}
	else
	{
		usart_set_flow_control(usart, USART_FLOWCONTROL_NONE);

	}

	// Enable USART.
	usart_enable(usart);
	return NULL;
}
