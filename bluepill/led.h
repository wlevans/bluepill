#ifndef BLUEPILL_LED
#define BLUEPILL_LED

#include <stdbool.h>

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
bool led_get(void);

#endif /* BLUEPILL_LED */
