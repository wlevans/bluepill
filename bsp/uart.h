#ifndef __BLUEPILL_USART__
#define __BLUEPILL_USART__

#include <stdint.h>
#include <stdbool.h>


// To do: Define common baud rates.

// Opaque USART structure (hides the structure from the user).
typedef struct usart_t usart_t;
// Handle (pointer) to USART.
typedef usart_t *usart_handle_t;

int32_t usart_init(uint32_t usart, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits, bool flowcontrol);

#endif /* __BLUEPILL_USART__ */
