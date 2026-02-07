CFLAGS ?= -Wall -Wextra

all: monitor

monitor: main.o systat.o
	gcc $(CFLAGS) -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -o $@ -c $<

clean:
	rm -rf *.o all

.PHONY: all clean