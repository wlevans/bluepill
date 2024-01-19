#ifndef BLUEPILL_USART
#define BLUEPILL_USART

#include "FreeRTOS.h"
#include "queue.h"

#include <stdint.h>
#include <stdbool.h>
#include <libopencm3/stm32/usart.h>

void usart1_init(void);

#endif /* BLUEPILL_USART */
