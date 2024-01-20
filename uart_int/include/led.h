#ifndef __BLUEPILL_LED__
#define __BLUEPILL_LED__

#include <stdbool.h>

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
bool led_get(void);

#endif /* __BLUEPILL_LED__ */
