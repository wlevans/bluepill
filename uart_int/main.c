#include "FreeRTOS.h"
#include "task.h"

#include "bluepill.h"
#include "usart.h"
#include "led.h"

#include <stdint.h>

uint32_t uart = 0;

static char *menu[] = {"\x1B[2J\x1B[H",
		"1. LED On\n\r",
		"2. LED Off\n\r",
		"3. LED Toggle\n\n\r",
		"LED state: "};

static void process_cmd(void *args __attribute((unused)))
{
	static char data = 0;

	// To do: Should menu transmit be moved to main?
	// Transmit menu.
	for(uint8_t i = 0; i < sizeof(menu) / sizeof(menu[0]); ++i)
	{
		usart_puts(uart, menu[i]);
	}
	// Send LED state. 1 for on; 0 for off.
	usart_putc(uart, led_get()?'1':'0');
	// Position curser.
//	usart_puts(uart, "\x1B[6;0H");

	while(1)
	{
		// Wait until a command is received.
		if(usart_getc(uart, &data) != 0)
		{
			switch(data)
			{
				case '1':
					led_on();
					break;
				case '2':
					led_off();
					break;
				case '3':
					led_toggle();
					break;
				default:
					break;
			}
			// To do: Debug following usart_puts commands.
			// Update LED status.
			usart_puts(uart, "\x1B[5;12H");
//			usart_puts(uart, "\x1B[6;0H");
			usart_putc(uart, led_get()?'1':'0');
			usart_puts(uart, "\x1B[6;0H");
		}
		taskYIELD();
	}
	return;
}

int main(void)
{
	// Initialize board.
	board_init();
	// Initialize LED.
	led_init();
//	led_toggle();

	// Set up USART.
	uart = usart_init(1, 38400, 8, USART_PARITY_NONE, USART_STOPBITS_1, false);

	// To do: Move IRQ code to usart.*?
	// Enable USART RX and TX interrupts.
	usart_enable_rx_interrupt(uart);
	usart_enable_tx_interrupt(uart);
	// Make sure the interrupt is routed through the NVIC
	nvic_enable_irq(NVIC_USART1_IRQ);

	// Enable USART.
	usart_enable(uart);

	// Create FreeRTOS tasks.
	xTaskCreate(process_cmd, "process_cmd", 256, NULL, configMAX_PRIORITIES - 1, NULL);
	// Start FreeRTOS scheduler.
	vTaskStartScheduler();
	// Will only get here if the scheduler fails to start.
	while(1);
	return 0;
}
