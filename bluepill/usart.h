#ifndef BLUEPILL_USART
#define BLUEPILL_USART

#include <stdint.h>
#include <stdbool.h>

// Opaque USART structure (hides the structure from teh user).
typedef struct usart_t usart_t;
// Handle (pointer) to USART.
typedef usart_t *usart_handle_t;

usart_handle_t usart_init(uint32_t usart, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits, bool flowcontrol);

#endif /* BLUEPILL_USART */
