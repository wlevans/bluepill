#include <dma.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>

#define RX_BUFFER_SIZE  4
#define TX_BUFFER_SIZE  4

uint8_t uart_rx_buffer[RX_BUFFER_SIZE];
uint8_t uart_tx_buffer[TX_BUFFER_SIZE];

void dma1_init(void)
{
	// Enable clock.
	rcc_periph_clock_enable(RCC_DMA1);

//	// Configure DMA Channel 4 for USART1 TX.
//	dma_channel_reset(DMA1,DMA_CHANNEL4);
//	dma_set_priority(DMA1, DMA_CHANNEL4, DMA_CCR_PL_HIGH);
//	// Configure peripheral.
//	dma_set_peripheral_address(DMA1, DMA_CHANNEL4, (uint32_t)&USART1_DR);
//	dma_set_peripheral_size(DMA1, DMA_CHANNEL4, DMA_CCR_PSIZE_8BIT);
//	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL4);
//	// Configure memory.
//	dma_set_read_from_memory(DMA1,DMA_CHANNEL4);
//	dma_set_memory_address(DMA1, DMA_CHANNEL4, (uint32_t)uart_tx_buffer);
//	dma_set_number_of_data(DMA1, DMA_CHANNEL4, TX_BUFFER_SIZE);
//	dma_set_memory_size(DMA1,DMA_CHANNEL4,DMA_CCR_MSIZE_8BIT);
//	dma_enable_memory_increment_mode(DMA1,DMA_CHANNEL4);
//	// Enable DMA 1 Channel 4.
//	dma_enable_channel(DMA1, DMA_CHANNEL4);

	// Configure DMA Channel 5 for USART1 RX.
	dma_channel_reset(DMA1, DMA_CHANNEL5);
	dma_enable_circular_mode(DMA1, DMA_CHANNEL5);
	dma_set_priority(DMA1, DMA_CHANNEL5, DMA_CCR_PL_HIGH);
	// Configure peripheral.
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL5);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL5, (uint32_t)&USART1_DR);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL5, DMA_CCR_PSIZE_8BIT);
	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL5);
	// Configure memory.
	dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)uart_rx_buffer);
	dma_set_number_of_data(DMA1, DMA_CHANNEL5, RX_BUFFER_SIZE);
	dma_set_memory_size(DMA1, DMA_CHANNEL5, DMA_CCR_MSIZE_8BIT);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL5);
	// Enable interrupts.
	dma_enable_half_transfer_interrupt(DMA1, DMA_CHANNEL5);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);
	// Set interrupt priority.
	nvic_set_priority(NVIC_DMA1_CHANNEL5_IRQ, 0xCF);
	// Enable interrupt.
	nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ);
	// Enable DMA 1 Channel 5.
	dma_enable_channel(DMA1, DMA_CHANNEL5);

	return;
}

void dma1_channel5_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return;
}
