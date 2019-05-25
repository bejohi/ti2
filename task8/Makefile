.PHONY: all clean

CC := gcc
BINDIR := bin
EXEC := parking_lot
HEADERS := semaphore.h
SRCS := $(wildcard *.c)
OBJS := $(addprefix $(BINDIR)/,$(subst .c,.o,$(SRCS)))
CFLAGS := -D_DEFAULT_SOURCE -Wall -Wextra -pedantic -Werror -O0 -std=c99 -g -pthread
LDFLAGS := -g -pthread

all: $(BINDIR) $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(BINDIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BINDIR):
	mkdir -p $(BINDIR)

clean:
	rm -f $(BINDIR)/*.o $(EXEC)
