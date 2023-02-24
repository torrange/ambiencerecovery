#!/usr/bin/make -f

include Makefile.mk

PREFIX  ?= /usr/local
DESTDIR ?=


all: build

build: ambiencerecovery.c
	$(CC) $^ $(BUILD_C_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o ambiencerecovery.lv2/ambiencerecovery$(LIB_EXT)

clean:
	rm -f ambiencerecovery.lv2/ambiencerecovery$(LIB_EXT)

install:
	sudo install -d $(DESTDIR)$(PREFIX)/lib/lv2/ambiencerecovery.lv2
	sudo install -m 644 ambiencerecovery.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/ambiencerecovery.lv2/
	sudo install -m 644 ambiencerecovery.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/ambiencerecovery.lv2/

uninstall:
	sudo rm -rf $(DESTDIR)$(PREFIX)/lib/lv2/ambiencerecovery.lv2
