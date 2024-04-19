#ifndef __BLUEPILL_UART_DMA__
#define __BLUEPILL_UART_DMA__

#include <stdint.h>
#include <stddef.h>

#define RX_BUFFER_SIZE  8
#define TX_BUFFER_SIZE  8

void uart1_init(void);
void usart_rx(void *args __attribute((unused)));
void usart_tx(void *args __attribute((unused)));
void usart_process_data(const uint8_t * data, const size_t length);

#endif /* __BLUEPILL_UART_DMA__ */
