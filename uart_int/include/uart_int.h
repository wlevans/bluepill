#ifndef __BLUEPILL_UART_INT__
#define __BLUEPILL_UART_INT__

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/usart.h>

// To do: Define common baud rates.

// Opaque USART structure (hides the structure from teh user).
typedef struct usart_t usart_t;

void uart1_init(void);
uint32_t uart_puts(uint32_t usart, const char * src);
uint32_t uart_putc(uint32_t usart, char c);
uint32_t uart_getc(char * c);

#endif /* __BLUEPILL_UART_INT__ */
