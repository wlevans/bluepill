#include "uart_dma.h"
#include "ring_buffer.h"

#include <string.h>

#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"
#include "timers.h"

#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/dma.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>

#define EVENT_RX_IRQ_IDLE      0x001
#define EVENT_RX_IRQ_HALF_TX   0x002
#define EVENT_RX_IRQ_TX_COMPL  0x004
#define EVENT_TX_IRQ_TX_COMPL  0x008
#define EVENT_DATA_READY       0x010

// Event group.
EventGroupHandle_t uart_dma_eventgroup;

// USART RX and TX transfer buffers.
uint8_t uart_rx_buffer[RX_BUFFER_SIZE];
uint8_t uart_tx_buffer[TX_BUFFER_SIZE];

// USART TX ring buffer.
uint8_t uart_tx_rbuf_data[TX_RBUF_SIZE];
rbuf_t uart_tx_rbuf;

void uart1_init(void)
{
	// Create event group.
	uart_dma_eventgroup = xEventGroupCreate();
	// Initialize TX ring buffer.
	uart_tx_rbuf = rbuf_init(&uart_tx_rbuf_data, TX_RBUF_SIZE);

	// Enable clocks.
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_USART1);
	rcc_periph_clock_enable(RCC_DMA1);

	// Configure USART 1.
	gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
	gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART1_RX);
	usart_set_baudrate(USART1, 38400);
	usart_set_databits(USART1, 8);
	usart_set_parity(USART1, USART_PARITY_NONE);
	usart_set_stopbits(USART1, USART_STOPBITS_1);
	usart_set_mode(USART1, USART_MODE_TX_RX);
	usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
	usart_enable_idle_interrupt(USART1);
	nvic_set_priority(NVIC_USART1_IRQ, 0xCF);
	nvic_enable_irq(NVIC_USART1_IRQ);
	usart_enable(USART1);

	// Configure DMA Channel 4 for USART1 TX.
	dma_channel_reset(DMA1, DMA_CHANNEL4);
	dma_set_priority(DMA1, DMA_CHANNEL4, DMA_CCR_PL_HIGH);
	dma_set_read_from_memory(DMA1, DMA_CHANNEL4);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL4, (uint32_t)&USART1_DR);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL4, DMA_CCR_PSIZE_8BIT);
	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL4);
	dma_set_memory_size(DMA1, DMA_CHANNEL4, DMA_CCR_MSIZE_8BIT);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL4);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);
	nvic_set_priority(NVIC_DMA1_CHANNEL4_IRQ, 0xCF);
	nvic_enable_irq(NVIC_DMA1_CHANNEL4_IRQ);
	usart_enable_tx_dma(USART1);

	// Configure DMA Channel 5 for USART1 RX.
	dma_channel_reset(DMA1, DMA_CHANNEL5);
	dma_enable_circular_mode(DMA1, DMA_CHANNEL5);
	dma_set_priority(DMA1, DMA_CHANNEL5, DMA_CCR_PL_HIGH);
	dma_set_read_from_peripheral(DMA1, DMA_CHANNEL5);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL5, (uint32_t)&USART1_DR);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL5, DMA_CCR_PSIZE_8BIT);
	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL5);
	dma_set_memory_address(DMA1, DMA_CHANNEL5, (uint32_t)uart_rx_buffer);
	dma_set_number_of_data(DMA1, DMA_CHANNEL5, RX_BUFFER_SIZE);
	dma_set_memory_size(DMA1, DMA_CHANNEL5, DMA_CCR_MSIZE_8BIT);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL5);
	dma_enable_half_transfer_interrupt(DMA1, DMA_CHANNEL5);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);
	nvic_set_priority(NVIC_DMA1_CHANNEL5_IRQ, 0xCF);
	nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ);
	dma_enable_channel(DMA1, DMA_CHANNEL5);
	usart_enable_rx_dma(USART1);

	return;
}

void usart_rx(void *args __attribute((unused)))
{
	static uint16_t rx_buffer_tail = 0;	// Tail of the UART RX buffer.
	uint16_t rx_buffer_head;			// Head of the UART RX buffer.

	while(1)
	{
		// Wait for an interrupt to change one of the event group bits.
		xEventGroupWaitBits(uart_dma_eventgroup,
				EVENT_RX_IRQ_IDLE | EVENT_RX_IRQ_HALF_TX | EVENT_RX_IRQ_TX_COMPL,
				pdTRUE,
				pdFALSE,
				portMAX_DELAY);

		// Data is present in UART RX buffer. Update head position.
		rx_buffer_head = RX_BUFFER_SIZE - dma_get_number_of_data(DMA1, DMA_CHANNEL5);

		if(rx_buffer_head > rx_buffer_tail)
		{
			// Linear mode.
			usart_process_data(&uart_rx_buffer[rx_buffer_tail], rx_buffer_head - rx_buffer_tail);
		}
		else if(rx_buffer_head < rx_buffer_tail)
		{
			// Overflow mode.
			// Process data at the end of the USART RX buffer.
			usart_process_data(&uart_rx_buffer[rx_buffer_tail], RX_BUFFER_SIZE - rx_buffer_tail);
			// Process data at the beginning of the USART RX buffer.
			usart_process_data(&uart_rx_buffer[0], rx_buffer_head);
		}
		// Update tail position.
		rx_buffer_tail = rx_buffer_head;
	}
}

void usart_tx(void *args __attribute((unused)))
{
	while(1)
	{
		// Wait for a change in one of the event gropu bits.
		// EVENT_TX_IRQ_TX_COMPL indicates the previous transmit is complet.
		// EVENT_DATA_READY indicates new data is ready.
		xEventGroupWaitBits(uart_dma_eventgroup,
				EVENT_TX_IRQ_TX_COMPL | EVENT_DATA_READY,
				pdTRUE,
				pdFALSE,
				portMAX_DELAY);

		// Test if transfer is currently in-active.
		if(0 == dma_get_number_of_data(DMA1, DMA_CHANNEL4) &&
           !rbuf_empty(&uart_tx_rbuf))
		{
			// Disable DMA channel.
			dma_disable_channel(DMA1, DMA_CHANNEL4);
			// Determine length of transfer.
			size_t length = rbuf_size(&uart_tx_rbuf);
			if(length > TX_BUFFER_SIZE)
			{
				length = TX_BUFFER_SIZE;
			}
			dma_set_number_of_data(DMA1, DMA_CHANNEL4, length);
			// Get data to transfer.
			rbuf_pop(&uart_tx_rbuf, uart_tx_buffer, length);
			// Set memory address.
			dma_set_memory_address(DMA1, DMA_CHANNEL4, (uint32_t)uart_tx_buffer);
			// Start transfer.
			dma_enable_channel(DMA1, DMA_CHANNEL4);
		}
	}
	return;
}

void usart_process_data(uint8_t * data, size_t length)
{
	// If length is zero then return.
	if(0 == length)
	{
		return;
	}
	// Process data by putting it into the TX ring buffer.
	rbuf_push(&uart_tx_rbuf, data, length);
	// Set data ready bit.
	xEventGroupSetBits(uart_dma_eventgroup, EVENT_DATA_READY);

	return;
}

void usart1_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Test for idle line interrupt.
	if(usart_get_flag(USART1, USART_SR_IDLE))
	{
		// Clear interrupt. This is done by a read from the status register
		// followed by read from the data register.
		USART_SR(USART1);
		USART_DR(USART1);

		// Set idle bit.
		xEventGroupSetBitsFromISR(uart_dma_eventgroup, EVENT_RX_IRQ_IDLE, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	return;
}

void dma1_channel4_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Test for transfer complete interrupt.
	if(dma_get_interrupt_flag(DMA1, DMA_CHANNEL4, DMA_TCIF))
	{
		// Set transfer complete bit.
		xEventGroupSetBitsFromISR(uart_dma_eventgroup, EVENT_TX_IRQ_TX_COMPL, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL4, DMA_TCIF);
	}

	return;
}

void dma1_channel5_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Test for half transfer complete interrupt.
	if(dma_get_interrupt_flag(DMA1, DMA_CHANNEL5, DMA_HTIF))
	{
		// Set half transfer bit.
		xEventGroupSetBitsFromISR(uart_dma_eventgroup, EVENT_RX_IRQ_HALF_TX, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL5, DMA_HTIF);
	}
	// Test for transfer complete interrupt.
	if(dma_get_interrupt_flag(DMA1, DMA_CHANNEL5, DMA_TCIF))
	{
		// Set transfer complete bit.
		xEventGroupSetBitsFromISR(uart_dma_eventgroup, EVENT_RX_IRQ_TX_COMPL, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		dma_clear_interrupt_flags(DMA1, DMA_CHANNEL5, DMA_TCIF);
	}

	return;
}
