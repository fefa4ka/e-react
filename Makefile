DEVICE     = stm8l105k6 
CLOCK      = 16000000
PROGRAMMER = -c stlinkv2 

BUILD_NUMBER=$(shell cat BUILD.num)

BUILD_DIR ?= ./build

TARGET_DIR = ./target
COMPONENTS_DIR = ./component
SOURCES_DIR = .

CC = sdcc
MACHINE_FLAGS = -lstm8 -mstm8 --std-sdcc11 --stack-auto

VPATH = react:target:component

SOURCES := $(shell find $(SOURCES_DIR) -name "*.c" -or -name "*.s")
OBJECTS ?= $(SOURCES:%.c=%.o)
DEPENDENCIES := $(OBJECTS:.o=.d)

INCLUDE_DIRS := $(shell find $(SOURCES_DIR) -type f -name '*.c' -or -name '*.s' | sed -E 's|\/[^\/]+$$||' |sort -u)
INCLUDE_FLAGS := $(addprefix -I, $(INCLUDE_DIRS)) -I/usr/local/include/simavr/avr -I/usr/avr/include

TARGET_SOURCES = $(wildcard $(TARGET_DIR)/**/main.c)
TARGET_OBJECTS = $(patsubst %.c, %.o, \
			   $(TARGET_SOURCES))
TARGET ?= $(patsubst %.c, %, \
		 	$(TARGET_SOURCES))
PRODUCT_NAME ?= unnamed

COMPONENTS_SOURCES = $(shell find $(COMPONENTS_DIR) -name "*.c" -or -name "*.s")
COMPONENTS = $(patsubst %.c, %.o, \
			 	$(COMPONENTS_SOURCES))

DEFINE_FLAGS = -DBUILD_NUM=$(BUILD_NUMBER) -DARCH_$(ARCH)=1 -DPRODUCT_NAME=$(PRODUCT_NAME)
CFLAGS ?= -Wall $(OPTIMIZATION_FLAGS) $(DEBUG_FLAGS) $(DEFINE_FLAGS) $(MACHINE_FLAGS) 
COMPILE = $(CC) $(CFLAGS)

CFLAGS   = -mstm8 --std-sdcc11 --stack-auto
LIBS     = -lstm8 

# This just provides the conventional target name "all"; it is optional
# Note: I assume you set PNAME via some means not exhibited in your original file
all: $(PNAME)

# How to build the overall program
%.ihx: $(RELS)
	$(CC) -o $(dir $@) $(INCLUDE_FLAGS) $(CFLAGS) $*.c $(RELS) $(LIBS)

%.e.c: %.c
	@echo "Pressing $*.c"
	$(COMPILE) -c $*.c -o $@.expanded $(INCLUDE_FLAGS) -E
	grep ^[^\#].*$  $@.expanded > $@.unformatted
	clang-format $@.unformatted > $@
	rm $@.*

# How to build any .rel file from its corresponding .c file
# GNU would have you use a pattern rule for this, but that's GNU-specific
%.rel:
	$(CC) -o $(dir $@) -c $(INCLUDE_FLAGS) $(CFLAGS) $*.c

%.d: %.c
	$(CC) $(CFLAGS) $< -MM $(INCLUDE_FLAGS) > $@

clean:
	rm -rf $(TARGET) $(OBJECTS) $(DEPENDENCIES)
	find . -type f -name '*.map' -or -name '*.sym' -or -name '*.ihx' -or -name '*.asm' -or -name '*.rst' -or -name '*.rel' -or -name '*.lst' -or -name '*.elf' -or -name '*.e.c' -or -name '*.hex' | xargs rm -rf

# Suffixes appearing in suffix rules we care about.
# Necessary because .rel is not one of the standard suffixes.
.SUFFIXES: .c .rel .d

