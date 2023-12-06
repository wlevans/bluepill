# Makefile

# Include compiler tools.
include arm-none-eabi.mk

# ST-Link utility.
STFLASH = $(shell which st-flash)

# Output file name.
BINARY = uart

# Include path(s).
LIBOPENCM3 := ../libopencm3
RTOS := ../rtos
BLUEPILL := ../bluepill

# Source files.
SRCFILES  = main.c
SRCFILES += ${RTOS}/heap_3.c ${RTOS}/list.c ${RTOS}/port.c ${RTOS}/queue.c ${RTOS}/tasks.c
SRCFILES += ${BLUEPILL}/usart.c ${BLUEPILL}/bluepill.c

# Object file(s) to build.
TEMP1 = $(patsubst %.c,%.o,$(SRCFILES))
TEMP2 = $(patsubst %.cpp,%.o,$(TEMP1))
OBJS  = $(patsubst %.asm,%.o,$(TEMP2))

# Define linker script.
LDSCRIPT = stm32f103c8t6.ld

# Define architecture (chip).
LIBNAME   = opencm3_stm32f1
DEFS      = -DSTM32F1
FLASHSIZE = --flash=128k

# Architecture (chip) specific flags.
ARCH_FLAGS  = -mcpu=cortex-m3
ARCH_FLAGS += -mthumb
ARCH_FLAGS += -msoft-float
ARCH_FLAGS += -mfix-cortex-m3-ldrd

# Assembler flags.
ASFLAGS  = -mthumb
ASFLAGS += -mcpu=cortex-m3

# Common flags.
OPT    := -Os
DEBUG  := -g
CSTD   ?= -std=c99
CXXSTD ?= -std=c++11

# Target C flags.
TGT_CFLAGS += $(OPT) $(DEBUG) $(CSTD)
TGT_CFLAGS += $(ARCH_FLAGS)
TFT_CFLAGS += -Wimpliciit-function-declaration
TGT_CFLAGS += -Wredundant-decls
TGT_CFLAGS += -Wmissing-prototypes
TGT_CFLAGS += -Wstrict-prototypes

# Target C++ flags.
TGT_CXXFLAGS += $(OPT) $(DEBUG) $(CXXSTD)
TGT_CXXFLAGS += $(ARCH_FLAGS)
TGT_CXXFLAGS +=  -Weffc++

# C/C++ preprocessor common flags.
TGT_CPPFLAGS += $(DEFS)
TGT_CPPFLAGS += -MD
TGT_CPPFLAGS += -Wall
TGT_CPPFLAGS += -Wundef
TGT_CPPFLAGS += -fno-common
TGT_CPPFLAGS += -ffunction-sections
TGT_CPPFLAGS += -fdata-sections
TGT_CPPFLAGS += -Wextra
TGT_CPPFLAGS += -Wshadow
TGT_CPPFLAGS += -Wredundant-decls
TGT_CPPFLAGS += -I.
TGT_CPPFLAGS += -I./$(LIBOPENCM3)/include
TGT_CPPFLAGS += -I./${RTOS}
TGT_CPPFLAGS += -I./${BLUEPILL}

# Linker flags.
TGT_LDFLAGS += --static
TGT_LDFLAGS += -nostartfiles
TGT_LDFLAGS += -T$(LDSCRIPT)
TGT_LDFLAGS += $(ARCH_FLAGS)
TGT_LDFLAGS += $(DEBUG)
TGT_LDFLAGS += -Wl,-Map=$(*).map
TGT_LDFLAGS += -Wl,--cref
TGT_LDFLAGS += -Wl,--gc-sections

# Used libraries.
LDLIBS += -specs=nosys.specs
LDLIBS += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
LDLIBS += -L$(LIBOPENCM3)/lib
LDLIBS += -l$(LIBNAME)

.SUFFIXES: .elf .bin .hex .srec .list .map .images
.SECONDEXPANSION:
.SECONDARY:

all : elf
elf  : $(BINARY).elf
bin  : $(BINARY).bin
hex  : $(BINARY).hex
srec : $(BINARY).srec
list : $(BINARY).list

%.bin : %.elf
	$(OBJCOPY) -Obinary $(*).elf $(*).bin

%.hex : %.elf
	$(OBJCOPY) -Oihex $(*).elf $(*).hex

%.srec : %.elf
	$(OBJCOPY) -Osrec $(*).elf $(*).srec

%.list : %.elf
	$(OBJCOPY) -S $(*).elf $(*).list

%.elf %.map : $(OBJS) $(LDSCRIPT)
	$(CC) $(TGT_LDFLAGS) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(*).elf
	$(SIZE) $(BINARY).elf

%.o : %.c
	$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

%.o : %.cxx
	$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cxx

%.o : %.cpp
	$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cpp

%.o : %.asm
	$(AS) $(ASFLAGS) -o $*.o -c $<

clean :
	$(RM) *.o *.d generated.* $(OBJS) $(patsubst %.o,%.d,$(OBJS))

clobber : clean
	$(RM) -f *.elf *.bin *.hex *srec *.list *.map

# Flash device.
flash : $(BINARY).bin
	$(STFLASH) $(FLASHSIZE) --reset write $(BINARY).bin 0x08000000

.PHONY : images clean elf bin hex srec list all

# What does this do?
-include $(OBJS:.o=.d)
