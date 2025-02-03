PROG = http
SRCS = main.c mongoose.c
DEBS = mongoose.h

CLEANFILES = $(PROG)

# Add / change option in CFLAGS and LDFLAGS
CFLAGS += -Wall $(shell pkg-config)
LDFLAGS += $(shell pkg-config)

all: $(PROG)

$(PROG): $(SRCS) $(DEPS)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

clean:
	rm -f $(CLEANFILES) $(patsubst %.c,%.o, $(SRCS))
