#ifndef BLUEPILL_USART
#define BLUEPILL_USART

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/usart.h>

// To do: Define common baud rates.

// Opaque USART structure (hides the structure from teh user).
typedef struct usart_t usart_t;

void usart1_init(void);
uint32_t usart_puts(uint32_t usart, const char * src);
uint32_t usart_putc(uint32_t usart, char c);
uint32_t usart_getc(char * c);

#endif /* BLUEPILL_USART */
