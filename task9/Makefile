.PHONY: all clean

CC := gcc
OBJDIR := obj
SRCS := $(filter-out processes_%,$(wildcard *.c))
OBJS := $(addprefix $(OBJDIR)/,$(subst .c,.o,$(SRCS)))
EXECS := $(subst processes_,,$(subst .c,,$(wildcard processes_*.c)))
HEADERS := simul_os.h core.h
CFLAGS := -D_DEFAULT_SOURCE -Wall -Wextra -pedantic -Werror -O0 -std=c11 -g -pthread
LDFLAGS := -g -pthread

all: $(OBJDIR) $(EXECS)

%: $(OBJS) $(OBJDIR)/processes_%.o
	$(CC) $(LDFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR):
	mkdir -p $(OBJDIR)

debug:
	@echo "OBJS : $(OBJS)"
	@echo "EXECS : $(EXECS)"

clean:
	rm -f $(OBJDIR)/*.o $(EXECS)
