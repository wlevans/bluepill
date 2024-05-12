#include "led.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

#define LED_PORT	(uint32_t)GPIOC
#define LED_PIN		(uint16_t)GPIO13
#define LED_CLOCK	RCC_GPIOC

void led_init(void)
{
  // Set up LED clock.
  rcc_periph_clock_enable(LED_CLOCK);
  // Set up LED.
  gpio_set_mode(LED_PORT, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, LED_PIN);
  return;
}

inline void led_on(void)
{
  gpio_clear(LED_PORT, LED_PIN);
  return;
}

inline void led_off(void)
{
  gpio_set(LED_PORT, LED_PIN);
  return;
}

inline void led_toggle(void)
{
  gpio_toggle(LED_PORT, LED_PIN);
  return;
}

inline bool led_get(void)
{
  return !gpio_get(LED_PORT, LED_PIN);
}
