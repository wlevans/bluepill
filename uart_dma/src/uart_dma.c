#include "uart_dma.h"

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

#define EVENT_RX_IRQ_IDLE     0x001
#define EVENT_RX_IRQ_HALF_TX  0x002
#define EVENT_RX_IRQ_TX_COMPL 0x004
#define EVENT_TX_IRQ_TX_COMPL 0x008
#define EVENT_RINGBUFFER_RDY  0x010

uint8_t uart_rx_buffer[RX_BUFFER_SIZE];
uint8_t uart_tx_buffer[TX_BUFFER_SIZE];

// Event group.
EventGroupHandle_t uart_dma_eventgroup;

void uart1_init(void)
{
	// Create event group.
	uart_dma_eventgroup = xEventGroupCreate();

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

	// Configure DMA Channel 4 for USART1 TX.
	dma_channel_reset(DMA1, DMA_CHANNEL4);
	dma_set_priority(DMA1, DMA_CHANNEL4, DMA_CCR_PL_HIGH);
	dma_set_read_from_memory(DMA1, DMA_CHANNEL4);
	dma_set_peripheral_address(DMA1, DMA_CHANNEL4, (uint32_t)&USART1_DR);
	dma_set_peripheral_size(DMA1, DMA_CHANNEL4, DMA_CCR_PSIZE_8BIT);
	dma_disable_peripheral_increment_mode(DMA1, DMA_CHANNEL4);
	dma_set_memory_size(DMA1, DMA_CHANNEL4, DMA_CCR_MSIZE_8BIT);
	dma_enable_memory_increment_mode(DMA1, DMA_CHANNEL4);

	// Configure and enable interrupts.
	usart_enable_idle_interrupt(USART1);
	nvic_set_priority(NVIC_USART1_IRQ, 0xCF);
	nvic_enable_irq(NVIC_USART1_IRQ);
	dma_enable_half_transfer_interrupt(DMA1, DMA_CHANNEL5);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL5);
	nvic_set_priority(NVIC_DMA1_CHANNEL5_IRQ, 0xCF);
	nvic_enable_irq(NVIC_DMA1_CHANNEL5_IRQ);
	dma_enable_transfer_complete_interrupt(DMA1, DMA_CHANNEL4);
	nvic_enable_irq(NVIC_DMA1_CHANNEL4_IRQ);

	// Enable USART and DMA.
	dma_enable_channel(DMA1, DMA_CHANNEL5);
	usart_enable_rx_dma(USART1);
	usart_enable(USART1);

	return;
}

void usart_rx(void *args __attribute((unused)))
{
	static uint16_t rx_buffer_tail = 0;	// Tail of the UART RX buffer.
	uint16_t rx_buffer_head;				// Head of the UART RX buffer.

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
		// To do: Is this test necessary?
		if(rx_buffer_tail == RX_BUFFER_SIZE)
		{
			rx_buffer_tail = 0;
		}
	}
}

void usart_tx(void *args __attribute((unused)))
{
	while(1)
	{
		// Wait for an interrupt to change one of the event group bits.
		xEventGroupWaitBits(uart_dma_eventgroup,
				EVENT_TX_IRQ_TX_COMPL | EVENT_RINGBUFFER_RDY,
				pdTRUE,
				pdFALSE,
				portMAX_DELAY);
		// To do:
		// Test if transfer is currently in-active.
		// Transfer data in ringbuffer over UART.
	}
}

void usart_process_data(const uint8_t * data, const size_t length)
{
	// To do:
	// Add header to ringbuffer.
	// Add data to ringbuffer.
	// Add footer to ringbuffer.
	// Signal usart_tx task that there is data in the ringbuffer.

	char header[] = "\x1B[32m";
	char footer[] = "\x1B[39m\r\n";

	// To do: Move echoing to usart_tx task.
	// Transmit header.
	for(size_t i = 0; i < strlen(header); ++i)
	{
		while(!usart_get_flag(USART1, USART_SR_TXE))
		{
			taskYIELD();
		}
		usart_send(USART1, header[i]);
	}

	// Echo back message.
	for(size_t i = 0; i < length; ++i)
	{
		while(!usart_get_flag(USART1, USART_SR_TXE))
		{
			taskYIELD();
		}
		usart_send(USART1, data[i]);
	}

	// Transmit footer.
	for(size_t i = 0; i < strlen(footer); ++i)
	{
		while(!usart_get_flag(USART1, USART_SR_TXE))
		{
			taskYIELD();
		}
		usart_send(USART1, footer[i]);
	}

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

void dma1_channel5_isr(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	// Test for half transfer complete interrupt.
	if(dma_get_interrupt_flag(DMA1, DMA_CHANNEL5, DMA_HTIF))
	{
		// Set half transfer bit.
		xEventGroupSetBitsFromISR(uart_dma_eventgroup, EVENT_RX_IRQ_HALF_TX, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	// Test for transfer complete interrupt.
	if(dma_get_interrupt_flag(DMA1, DMA_CHANNEL5, DMA_TCIF))
	{
		// Set transfer complete bit.
		xEventGroupSetBitsFromISR(uart_dma_eventgroup, EVENT_RX_IRQ_TX_COMPL, &xHigherPriorityTaskWoken);
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
	dma_clear_interrupt_flags(DMA1, DMA_CHANNEL5, DMA_HTIF | DMA_TCIF);
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
	}
	dma_clear_interrupt_flags(DMA1, DMA_CHANNEL4, DMA_TCIF);
	return;
}
