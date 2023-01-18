.PHONY: all clean run debug

CC := clang
DEBUGGER := lldb
MKDIR := mkdir -p
SRCDIR := src
OBJDIR := obj
DEPDIR := dep
INCDIR := inc
SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(subst $(SRCDIR)/,$(OBJDIR)/,$(SRCS:.c=.o))
DEPS := $(subst $(SRCDIR)/,$(DEPDIR)/,$(SRCS:.c=.d))
BIN ?= main

CPPFLAGS := -I"$(INCDIR)" $(CPPFLAGS)
CFLAGS := -Wall -Wextra -Wpedantic -pipe -std=c17 $(CFLAGS)
LDFLAGS := -fuse-ld=mold -lm $(LDFLAGS)

LIBS :=

ifneq ($(strip $(LIBS)),)
ifeq ($(strip $(STATIC)),1)
CPPFLAGS += $(shell pkg-config --static --cflags-only-I $(LIBS))
CFLAGS += $(shell pkg-config --static --cflags-only-other $(LIBS))
LDFLAGS += $(shell pkg-config --static --libs $(LIBS))
else
CPPFLAGS += $(shell pkg-config --cflags-only-I $(LIBS))
CFLAGS += $(shell pkg-config --cflags-only-other $(LIBS))
LDFLAGS += $(shell pkg-config --libs $(LIBS))
endif
endif

DEBUGFLAGS ?= -g -glldb
SANFLAGS ?= -fsanitize=undefined,address
OPTIFLAGS ?= -O2
LTOFLAGS ?= -flto
STATICFLAGS ?= -static

ifeq ($(strip $(DEBUG)),1)
CFLAGS += $(DEBUGFLAGS)
endif
ifeq ($(strip $(SANITIZE)),1)
CFLAGS += $(SANFLAGS)
endif
ifeq ($(strip $(OPTI)),1)
CFLAGS += $(OPTIFLAGS)
endif
ifeq ($(strip $(LTO)),1)
CFLAGS += $(LTOFLAGS)
endif
ifeq ($(strip $(STATIC)),1)
LDFLAGS += $(STATICFLAGS)
endif

all: $(BIN)

$(DEPDIR)/%.d: $(SRCDIR)/%.c
	@$(MKDIR) "$(DEPDIR)"
	@$(CC) $(CPPFLAGS) -MM "$<" | sed 's,\($*\)\.o[ :]*,$(OBJDIR)/\1.o: ,g' > "$@"

include $(DEPS)

$(OBJDIR)/%.o: $(SRCDIR)/%.c
	@$(MKDIR) "$(OBJDIR)"
	$(CC) $(CPPFLAGS) $(CFLAGS) -c "$<" -o "$@"

$(BIN): $(OBJS)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(LDFLAGS) $^ -o "$@"

clean:
	$(RM) $(OBJDIR)/*.o $(DEPDIR)/*.d $(BIN)

run: $(BIN)
	./"$(BIN)" $(ARGS)

debug: $(BIN)
	@$(DEBUGGER) ./"$(BIN)"
