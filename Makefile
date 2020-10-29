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
	CC = clang 
endif
AVRDUDE = avrdude $(PROGRAMMER) -p $(DEVICE)

VPATH = react:target:component

SOURCES := $(shell find $(SOURCES_DIR) -name "*.c" -or -name "*.s")
OBJECTS ?= $(SOURCES:%.c=%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

INCLUDE_DIRS := $(shell find $(SOURCES_DIR) -type f -name '*.c' -or -name '*.s' | sed -E 's|\/[^\/]+$$||' |sort -u)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS))

TARGET_SOURCES = $(wildcard $(TARGET_DIR)/**/*.c)
TARGET_OBJECTS = $(patsubst %.c, %.o, \
			   $(TARGET_SOURCES))
TARGET ?= $(patsubst %.c, %, \
		 	$(TARGET_SOURCES))
PRODUCT_NAME ?= unnamed

COMPONENTS_SOURCES = $(shell find $(COMPONENTS_DIR) -name "*.c" -or -name "*.s")
COMPONENTS = $(patsubst %.c, %.o, \
			 	$(COMPONENTS_SOURCES))

ifeq ($(ARCH), AVR)
	OPTIMIZATION_FLAGS = -Os
	MACHINE_FLAGS = -DF_CPU=$(CLOCK) -mmcu=$(DEVICE)
else
	MACHINE_FLAGS = -DF_CPU=$(CLOCK)
endif
DEFINE_FLAGS = -DBUILD_NUM=$(BUILD_NUMBER) -DARCH_$(ARCH)=1 -DPRODUCT_NAME=$(PRODUCT_NAME)
CFLAGS ?= -g $(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) $(DEFINE_FLAGS) $(MACHINE_FLAGS) --std=gnu99 
COMPILE = $(CC) $(CFLAGS)

$(TARGET): $(OBJECTS) 
	@echo $(DEPENDENCIES)
	$(COMPILE) -o $@ $(OBJECTS)

%.map: % $(OBJECTS)
	$(CC) -g -mmcu=$(DEVICE) -Wl,-Map,$@ -o $* $(OBJECTS)
	avr-objdump -h -S $* > $*.lst

%.hex: % %.map
	rm -f $@
	avr-objcopy -j .text -j .data -O ihex $* $@
	avr-size --format=avr --mcu=$(DEVICE) $*

%.o: %.c
	$(CC) $(CFLAGS) -c $*.c -o $@ $(INCLUDE_FLAGS)

flash: $(TARGET)
	$(AVRDUDE) -U flash:w:${TARGET}:i

read: $(TARGET)
	$(AVRDUDE) -U read:w:${TARGET}:i

%.d: %.c
	$(CC) $(CFLAGS) $< -MM $(INCLUDE_FLAGS) > $@

clean:
	rm -rf $(TARGET) $(OBJECTS) $(DEPENDENCIES)
	find $(TARGET_DIR) -type f -name '*.map' -or -name '*.lst' -or -name '*.elf' -or -name '*.hex' | xargs rm -rf


MKDIR_P ?= mkdir -p

