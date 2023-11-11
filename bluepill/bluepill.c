#include "bluepill.h"

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>

inline void board_init(void)
{
	// Set up clock.
	rcc_clock_setup_pll(&rcc_hse_configs[RCC_CLOCK_HSE8_72MHZ]);

	return;
}
