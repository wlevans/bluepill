#ifndef BLUEPILL_USART
#define BLUEPILL_USART

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/usart.h>

// To do: Define common baud rates.

// To do: Better naming convention.
// USART_PARITY_XXXX is used by libopencm3.
// To do: #define or enum?
//#define PARITY_NONE 0
//#define PARITY_EVEN 1
//#define PARITY_ODD  3
enum PARITY
{
	PARITY_NONE = 0,
	PARITY_EVEN = 1,
	PARITY_ODD  = 2
};

// Opaque USART structure (hides the structure from teh user).
typedef struct usart_t usart_t;
// Handle (pointer) to USART.
typedef usart_t *usart_handle_t;

int32_t usart_init(uint32_t usart, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits, bool flowcontrol);

#endif /* BLUEPILL_USART */
