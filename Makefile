CC = gcc
CFLAGS = -s
LDFLAGS = -lm

CFILES = $(shell find ./ -name "*.c")
OBJS = $(CFILES:.c=.o)
DFILES = $(OBJS:.o=.d)

.PHONY: clean

main.exe: $(OBJS)
	 $(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o main.exe

%.d: %.c
	$(CC) -MM $< > $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DFILES)
endif

clean:
	rm -f $(OBJS) $(DFILES)
	rm -f *.exe
