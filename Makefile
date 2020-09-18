DEVICE     = atmega168p
CLOCK      = 16000000
PROGRAMMER = -c 2ftbb 
FUSES      = -U lfuse:w:0xee:m -U hfuse:w:0xdf:m

BUILD_NUMBER=$(shell cat BUILD.num)


BUILD_DIR ?= ./build

TARGET_DIR = ./target
COMPONENTS_DIR = ./component
SOURCES_DIR = .

ARCH ?= x86
ifeq ($(ARCH), AVR)
	CC = avr-gcc
else
	CC = gcc
endif
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

VPATH = react:target:tests:component

SOURCES := $(shell find $(SOURCES_DIR) -name "*.c" -or -name "*.s")
OBJECTS ?= $(SOURCES:%.c=%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

INCLUDE_DIRS := $(shell find $(SOURCES_DIR) -type f -name '*.c' -or -name '*.s' | sed -E 's|\/[^\/]+$$||' |sort -u)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS))

TARGET_SOURCES = $(wildcard $(TARGET_DIR)/*.c)
TARGET_OBJECTS = $(patsubst %.c, %.o, \
			   $(TARGET_SOURCES))
TARGET ?= $(patsubst %.c, %, \
		 	$(TARGET_SOURCES))

COMPONENTS_SOURCES = $(shell find $(COMPONENTS_DIR) -name "*.c" -or -name "*.s")
COMPONENTS = $(patsubst %.c, %.o, \
			 	$(COMPONENTS_SOURCES))

DEBUG_FLAGS = -Wall
OPTIMIZATION_FLAGS = -Os
ifeq ($(ARCH), AVR)
	MACHINE_FLAGS = -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
else
	MACHINE_FLAGS = -DF_CPU=$(CLOCK)
endif
DEFINE_FLAGS = -DBUILD_NUM=$(BUILD_NUMBER) -DARCH_$(ARCH)=1 
CFLAGS ?= $(INCLUDE_FLAGS) $(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) $(DEFINE_FLAGS) $(MACHINE_FLAGS) --std=gnu99 
COMPILE = $(CC) $(CFLAGS)

%.elf: $(OBJECTS) 
	@echo $(DEPENDENCIES)
	$(COMPILE) -o $@ $(OBJECTS)

%.map: %.elf $(OBJECTS)
	$(CC) -g -mmcu=$(DEVICE) -Wl,-Map,$@ -o $*.elf $(OBJECTS)
	avr-objdump -h -S $*.elf > $*.lst 

%.hex: %.elf %.map
	rm -f $@
	avr-objcopy -j .text -j .data -O ihex $*.elf $@
	avr-size --format=avr --mcu=$(DEVICE) $*.elf

%: %.o $(OBJECTS)
	$(COMPILE) -o $@ $(OBJECTS)

flash: $(TARGET)
	$(AVRDUDE) -U flash:w:${TARGET}:i

read: $(TARGET)
	$(AVRDUDE) -U read:w:${TARGET}:i

.o:
	$(COMPILE) -c $< -o $@

%.d: %.c
	$(CC) $(CFLAGS) $< -MM > $@

clean:
	rm -rf $(TARGET) $(OBJECTS) $(DEPENDENCIES)
	find $(TARGET_DIR) -type f -name '*.map' -or -name '*.lst' -or -name '*.elf' -or -name '*.hex' | xargs rm


MKDIR_P ?= mkdir -p

