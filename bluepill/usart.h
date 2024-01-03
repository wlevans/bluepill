#ifndef BLUEPILL_USART
#define BLUEPILL_USART

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

// To do: Define common baud rates.

// Opaque USART structure (hides the structure from teh user).
typedef struct usart_t usart_t;

//static QueueHandle_t uart_txq;
//static QueueHandle_t uart_rxq;

int32_t usart_init(uint32_t usart, uint32_t baudrate, uint32_t databits, uint32_t parity, uint32_t stopbits, bool flowcontrol);
uint32_t usart_puts(uint32_t usart, char * src);
uint32_t usart_putc(uint32_t usart, char src);
//void usart1_isr(void);

#endif /* BLUEPILL_USART */
