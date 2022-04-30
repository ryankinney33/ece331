obj-m+= lcd.o

TARGETS = write clear open locktest backwards
CFLAGS = -g -Wall -O2

.PHONY: all clean driver testprogs

all: driver testprogs

driver:
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

testprogs: $(TARGETS)

$(TARGETS): %: %.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	@$(MAKE) -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
	@$(RM) $(TARGETS) $(OBJS) core*
