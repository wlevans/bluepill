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

int32_t usart_init(uint32_t usart, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits, bool flowcontrol);
uint32_t usart_puts(uint32_t usart, const char * src);
uint32_t usart_putc(uint32_t usart, char c);
uint32_t usart_getc(uint32_t usart, char * c);

#endif /* BLUEPILL_USART */
