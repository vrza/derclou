WARN	= -pedantic -Wall -Wmissing-prototypes -Wmissing-declarations -Wsign-compare
OPT	= -O2 -funroll-loops
CFLAGS	= -I. -std=c99 $(OPT) $(WARN) -g $$($(SDL2_CONFIG) --cflags)
LDFLAGS = $$($(SDL2_CONFIG) --libs) -lm

SRCDIR  = .
BINARY  = $(BINDIR)/derclou

RM ?= rm

SOURCES != find $(SRCDIR) -name '*.c'
DIRS    != find $(SRCDIR) -mindepth 1 -maxdepth 1 -type d | cut -d/ -f2
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:%.c=%.o))

.PHONY: all clean

all: $(BINARY)


$(BINARY): $(BINDIR) $(OBJECTS)
	$(CC) -o $(BINARY) $(OBJECTS) $(LDFLAGS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

$(BINDIR):
	mkdir -p $(OBJDIR) && cd $(OBJDIR) && mkdir -p $(DIRS)

clean:
	$(RM) -r $(OBJDIR) $(BINDIR)
