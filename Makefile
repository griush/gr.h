CC := gcc
CFLAGS := -Wall -Wextra -Werror -std=c99 -g
AR := ar
ARFLAGS := rcs

ifneq ($(CONFIG),)
  ifeq ($(CONFIG), release)
    CFLAGS := -Wall -Wextra -Werror -std=c99 -O2
  endif
endif

PREFIX ?= /usr/local
INCLUDEDIR := $(PREFIX)/include
LIBDIR := $(PREFIX)/lib

LIB_SRC := gr.c
LIB_HEADER := gr.h
LIB_OBJ := gr.o
LIB_STATIC := libgr.a

DEMO_SRC := test/demo.c
DEMO_BIN := demo

TEST_SRC := test/test.c
TEST_BIN := tests

.PHONY: all clean install uninstall

all: $(DEMO_BIN) $(TEST_BIN)

$(LIB_OBJ): $(LIB_SRC) $(LIB_HEADER)
	$(CC) $(CFLAGS) -c $< -o $@

$(LIB_STATIC): $(LIB_OBJ)
	$(AR) $(ARFLAGS) $@ $<

$(DEMO_BIN): $(DEMO_SRC) $(LIB_STATIC)
	$(CC) $(CFLAGS) $^ -o $@

$(TEST_BIN): $(TEST_SRC) $(LIB_STATIC)
	$(CC) $(CFLAGS) $^ -o $@

install: $(LIB_STATIC)
	install -Dm644 $(LIB_STATIC) $(LIBDIR)/$(LIB_STATIC)
	install -Dm644 $(LIB_HEADER) $(INCLUDEDIR)/$(LIB_HEADER)

uninstall:
	rm -f $(LIBDIR)/$(LIB_STATIC)
	rm -f $(INCLUDEDIR)/$(LIB_HEADER)

clean:
	rm -f $(LIB_OBJ) $(LIB_STATIC) $(DEMO_BIN) $(TEST_BIN)
