CROSS_COMPILE  = arm-none-eabi

CXX     = $(CROSS_COMPILE)-g++
CC      = $(CROSS_COMPILE)-gcc
AS      = $(CROSS_COMPILE)-as
AR      = $(CROSS_COMPILE)-ar
NM      = $(CROSS_COMPILE)-nm
LD      = $(CROSS_COMPILE)-ld
OBJDUMP = $(CROSS_COMPILE)-objdump
OBJCOPY = $(CROSS_COMPILE)-objcopy
RANLIB  = $(CROSS_COMPILE)-ranlib
STRIP   = $(CROSS_COMPILE)-strip
SIZE    = $(CROSS_COMPILE)-size
GDB     = $(CROSS_COMPILE)-gdb
