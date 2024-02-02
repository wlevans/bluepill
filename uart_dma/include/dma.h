#ifndef __BLUEPILL_DMA__
#define __BLUEPILL_DMA__

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/dma.h>

// To do: Define common baud rates.

// Opaque USART structure (hides the structure from teh user).
typedef struct usart_t usart_t;

void dma1_init(void);

#endif /* __BLUEPILL_DMA__ */
