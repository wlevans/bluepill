#include "uart_dma.h"

#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

uint8_t uart_rx_buffer[RX_BUFFER_SIZE];
uint8_t uart_tx_buffer[TX_BUFFER_SIZE];

void uart1_init(void)
{
	// Enable clocks.
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	// Configure USART 1.
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	usart_set_baudrate(USART1, 38400);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	// Enable USART RX interrupt.
	usart_enable_rx_interrupt(USART1);
	// Set USART 1 interrupt priority.
	nvic_set_priority(NVIC_USART1_IRQ, 0xCF);
	// Enable USART 1 interrupt.
	nvic_enable_irq(NVIC_USART1_IRQ);
	// Enable USART.
	usart_enable(USART1);

	return;
}

void usart_rx(void *args __attribute((unused)))
{
	while(1)
	{
		taskYIELD();
	}
}

void usart_tx(void *args __attribute((unused)))
{
	while(1)
	{
		taskYIELD();
	}
}

void usart_process_data(const uint8_t * data, const size_t length)
{
	// To do:
	// Process data received over USART.

	for(size_t i = 0; i < length; ++i)
	{
		while(!usart_get_flag(USART1, USART_SR_TXE))
		{
			taskYIELD();
		}
		usart_send(USART1, data[i]);
	}

	return;
}

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

void usart1_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// To do:
	// Check for idle line interrupt.

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return;
}

void dma1_channel5_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// To do:
	// Check for half transfer complete interrupt.

	// To do:
	// check for transfer complete interrupt.

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	return;
}
