# make a c i p t

################################################################################

TARGET := m590e

DEBUG := 0
OPTIM := O1

ASRCS := $(sort $(shell find . -name '*.s' -printf '%f '))
AOBJS := $(addprefix objs/,$(ASRCS:.s=.o))

CSRCS := $(sort $(shell find src -name '*.c' -printf '%f '))
CDEPS := $(patsubst %.c,deps/%.d,$(CSRCS))
COBJS := $(addprefix objs/,$(CSRCS:.c=.o))

ASFLAGS := -Wa,--warn -Wa,--fatal-warnings
CPPFLAGS := -I inc -I inc/pt -I inc/uip -I inc/tlsf -I /usr/arm-none-eabi/include
CFLAGS := -march=armv6-m -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -mlittle-endian -ffreestanding -fsigned-char -fdata-sections -ffunction-sections -Wall -Werror -Wno-unused-but-set-variable -$(OPTIM)
LDFLAGS := -nostdlib -nostartfiles -nodefaultlibs -L/usr/arm-none-eabi/lib/thumb/v6-m/nofp -L/usr/lib/gcc/arm-none-eabi/9.1.0/thumb/v6-m/nofp -T $(TARGET).ld -Wl,-Map=$(TARGET).map -Wl,--cref -Wl,--gc-sections -Wl,--print-memory-usage -Wl,--stats -Wl,--print-output-form
LDLIBS := -lm -lc_nano -lgcc -lnosys

ifeq ($(DEBUG),1)
   CFLAGS += -g
endif

vpath %.h inc:inc/pt:inc/uip:inc/tlsf
vpath %.s src
vpath %.c src:src/uip:src/tlsf

$(shell if [ ! -d deps ]; then mkdir -p deps; fi)
$(shell if [ ! -d objs ]; then mkdir -p objs; fi)

################################################################################

$(TARGET).elf : $(AOBJS) $(COBJS)
	$(info Linking $(TARGET))
	arm-none-eabi-gcc $^ $(LDFLAGS) $(LDLIBS) -o $@
	arm-none-eabi-objdump -d -S -z -w $@ > $(TARGET).lst
	arm-none-eabi-objcopy -O ihex $@ $(TARGET).hex
	arm-none-eabi-objcopy -O binary $@ $(TARGET).bin
	find . -name '*.c' | xargs -n1 wc -l | sort -n -r | sed --silent '1,5p'
	arm-none-eabi-nm -S --size-sort -r $(TARGET).elf | sed --silent '1,5p'
	arm-none-eabi-size --format=sysv --common -d $(TARGET).elf

objs/%.o : %.s
	arm-none-eabi-gcc $< -c $(CFLAGS) $(ASFLAGS) -o $@

objs/%.o : %.c deps/%.d
	arm-none-eabi-gcc $< -c $(CPPFLAGS) $(CFLAGS) -o $@
	arm-none-eabi-gcc $< -E $(CPPFLAGS) -MM -MP -MT $@ -MF deps/$*.d && touch $@

deps/%.d : ;

.PRECIOUS : deps/%.d

################################################################################

.PHONY : a c i p t

a : $(TARGET).elf

c :
	rm -rf *.o *.elf *.bin *.hex *.map *.lst cscope* tags deps objs

i : a
	~/bin/lpc21isp -donotstart -verify -bin $(TARGET).bin /dev/ttyUSB0 115200 12000

p :
	picocom --baud 38400 --databits 8 --stopbits 1 --parity n --flow n --echo --quiet /dev/ttyUSB0

t :
	ctags -R --extras=+f *
	find . -name '*.[csh]' > cscope.files
	cscope -q -R -b -i cscope.files

################################################################################

-include $(CDEPS)
