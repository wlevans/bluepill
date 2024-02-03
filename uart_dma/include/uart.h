#ifndef __BLUEPILL_UART__
#define __BLUEPILL_UART__

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/usart.h>

void uart1_init(void);
void usart_rx(void *args __attribute((unused)));
void usart_tx(void *args __attribute((unused)));

#endif /* __BLUEPILL_UART__ */
