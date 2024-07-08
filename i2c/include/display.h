#ifndef INCLUDE_DISPLAY_H_
#define INCLUDE_DISPLAY_H_

#include <stdint.h>

void display_init(void);
void command(uint8_t const command);
void data(uint8_t data);

#endif /* INCLUDE_DISPLAY_H_ */
