#include <stdint.h>

#include "FreeRTOS.h"
#include "task.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/cm3/nvic.h>

#include "bsp.h"
#include "uart_int.h"
#include "led.h"

void printMenu(void);
static void process_cmd(void *args __attribute((unused)));

int main(void)
{
  // Initialize board.
  board_init();
  // Initialize LED.
  led_init();
  // Initialize USART 1.
  uart1_init();

  // Enable USART RX interrupt.
  usart_enable_rx_interrupt(USART1);
  // Set USART 1 interrupt priority.
  nvic_set_priority(NVIC_USART1_IRQ, 0xCF);
  // Enable USART 1 interrupt.
  nvic_enable_irq(NVIC_USART1_IRQ);

  // Enable USART.
  usart_enable(USART1);

  // Create FreeRTOS tasks.
  xTaskCreate(process_cmd, "process_cmd", 256, NULL, configMAX_PRIORITIES - 1, NULL);
  // Start FreeRTOS scheduler.
  vTaskStartScheduler();
  // Will only get here if the scheduler fails to start.
  while(1);
  return 0;
}

void printMenu(void)
{
  char *menu[] = {"\x1B[2J\x1B[0;0H",
      "1. LED On\n\r",
      "2. LED Off\n\r",
      "3. LED Toggle\n\n\r",
      "LED state: "};
  // Transmit menu.
  for(uint8_t i = 0; i < sizeof(menu) / sizeof(menu[0]); ++i)
  {
    uart_puts(USART1, menu[i]);
  }
}

static void process_cmd(void *args __attribute((unused)))
{
  static char data = 0;

  // Print (transmit) menu.
  printMenu();
  // Send LED state. 1 for on; 0 for off.
  uart_putc(USART1, led_get()?'1':'0');
  uart_puts(USART1, "\x1B[6;0H");

  while(1)
  {
    // Wait until a command is received.
    if(uart_getc(&data) != 0)
    {
      switch(data)
      {
        case '1':
          led_on();
          break;
        case '2':
          led_off();
          break;
        case '3':
          led_toggle();
          break;
        default:
          break;
      }
      // Update LED status.
      uart_puts(USART1, "\x1B[5;12H");
      uart_putc(USART1, led_get()?'1':'0');
      uart_puts(USART1, "\x1B[6;0H");
    }
    taskYIELD();
  }
  return;
}
