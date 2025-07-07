# Makefile

# Include compiler tools.
include arm-none-eabi.mk

# Output file name.
NAME = libbluepill
SNAME = $(NAME).a

# Include path(s).
LIBOPENCM3  := ../libopencm3

# Source files.
SRCFILES  = led.c

# Object file(s) to build.
TEMP1 = $(patsubst %.c,%.o,$(SRCFILES))
TEMP2 = $(patsubst %.cpp,%.o,$(TEMP1))
OBJS  = $(patsubst %.asm,%.o,$(TEMP2))

# Define architecture (chip).
LIBNAME   = opencm3_stm32f1
DEFS      = -DSTM32F1

# CFLAGS += -Wall -Werror -fPIC -ansi -pedantic

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
TFT_CFLAGS += -specs=nano.specs
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

#TGT_CPPFLAGS += -Wall -Werror -fPIC

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
LDLIBS += -specs=nano.specs
LDLIBS += -specs=nosys.specs
LDLIBS += -Wl,--start-group -lc -lgcc -lnosys -Wl,--end-group
LDLIBS += -L$(LIBOPENCM3)/lib
LDLIBS += -l$(LIBNAME)


ARFLAGS = rv


all : lib

lib : $(SNAME)


%.o : %.c
	$(CC) $(TGT_CFLAGS) $(CFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).c

%.o : %.cxx
	$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cxx

%.o : %.cpp
	$(CXX) $(TGT_CXXFLAGS) $(CXXFLAGS) $(TGT_CPPFLAGS) $(CPPFLAGS) -o $(*).o -c $(*).cpp

%.o : %.asm
	$(AS) $(ASFLAGS) -o $*.o -c $<

$(SNAME) : ${OBJS}
	# Generate static library.
	$(AR) $(ARFLAGS) ${SNAME}

clean :
	${RM} -f *.o *.d $(NAME).*
