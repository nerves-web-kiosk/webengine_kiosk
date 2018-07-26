all: submake

src/Makefile:
	cd src && $(NERVES_SYSTEM)/host/usr/bin/qmake kiosk.pro

submake: src/Makefile
	+$(MAKE) -C src install

clean:
	-rm -fr priv/kiosk src/Makefile src/*.o src/qrc_* src/ui_* src/moc_*

.PHONY: all clean submake

