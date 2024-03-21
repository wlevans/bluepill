#ifndef __BLUEPILL_UART_DMA__
#define __BLUEPILL_UART_DMA__

#define RX_BUFFER_SIZE  4
#define TX_BUFFER_SIZE  4

void uart1_init(void);
void usart_rx(void *args __attribute((unused)));
void usart_tx(void *args __attribute((unused)));

void dma1_init(void);

#endif /* __BLUEPILL_UART_DMA__ */
