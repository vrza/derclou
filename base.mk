SRCDIR  = src
SOURCES != find $(SRCDIR) -name '*.c'
DIRS    != find $(SRCDIR) -mindepth 1 -maxdepth 1 -type d | cut -d/ -f2
OBJECTS := $(addprefix $(OBJDIR)/,$(SOURCES:$(SRCDIR)/%.c=%.o))
BINARY  = $(BINDIR)/derclou

WARN	= -pedantic -Wall -Wmissing-prototypes -Wmissing-declarations -Wsign-compare
OPT	= -O2 -funroll-loops
CFLAGS	= -I$(SRCDIR) -std=c99 $(OPT) $(WARN) -g $$($(SDL2_CONFIG) --cflags)
LDFLAGS = $$($(SDL2_CONFIG) --libs) -lm

RM ?= rm -f

.PHONY: all clean

all: $(BINARY)

$(BINARY): $(BINDIR) $(OBJECTS)
	$(CC) -o $(BINARY) $(OBJECTS) $(LDFLAGS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -I$(HEADERDIR) -I$(dir $<) -c $< -o $@

$(BINDIR):
	mkdir -p $(OBJDIR) && cd $(OBJDIR) && mkdir -p $(DIRS)

clean:
	$(RM) -r $(OBJDIR) $(BINDIR)
