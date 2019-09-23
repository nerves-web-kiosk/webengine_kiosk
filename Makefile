# Override if using a specific Qt toolkit version
QMAKE ?= qmake

PREFIX = $(MIX_APP_PATH)/priv

all: $(PREFIX) submake assets

$(PREFIX):
	mkdir -p $@


src/Makefile:
	cd src && $(QMAKE) kiosk.pro

submake: src/Makefile
	+$(MAKE) -j3 -C src install

$(PREFIX)/assets: assets
	rsync -rupE $< $@

clean:
	-rm -fr priv/kiosk src/kiosk src/Makefile src/*.o src/qrc_* src/ui_* src/moc_* src/.qmake*

.PHONY: all clean submake

