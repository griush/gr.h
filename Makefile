CC := gcc
CFLAGS := -Wall -Wextra -Werror -O2 -std=c99
AR := ar
ARFLAGS := rcs

PREFIX ?= /usr/local
INCLUDEDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib

LIB_SRC := gr.c
LIB_HEADER := gr.h
LIB_OBJ := gr.o
LIB_STATIC := libgr.a

DEMO_SRC := test/demo.c
DEMO_BIN := demo

.PHONY: all clean install uninstall

all: $(DEMO_BIN)

$(LIB_OBJ): $(LIB_SRC) $(LIB_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_STATIC): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $<

$(DEMO_BIN): $(DEMO_SRC) $(LIB_STATIC)
	$(CC) $(CFLAGS) $^ -o $@

install: $(LIB_STATIC)
	install -Dm644 $(LIB_STATIC) $(LIBDIR)/$(LIB_STATIC)
	install -Dm644 $(LIB_HEADER) $(INCLUDEDIR)/$(LIB_HEADER)

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
	rm -f $(INCLUDEDIR)/$(LIB_HEADER)

clean:
	rm -f $(LIB_OBJ) $(LIB_STATIC) $(DEMO_BIN)
