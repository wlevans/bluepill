#ifndef __BLUEPILL_LED__
#define __BLUEPILL_LED__

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void led_init(void);
void led_on(void);
void led_off(void);
void led_toggle(void);
bool led_get(void);

#ifdef __cplusplus
}
#endif

#endif /* __BLUEPILL_LED__ */
