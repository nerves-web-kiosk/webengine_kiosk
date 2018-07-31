all: submake

src/Makefile:
	cd src && qmake -d kiosk.pro

submake: src/Makefile
	+$(MAKE) -C src install

clean:
	-rm -fr priv/kiosk src/kiosk src/Makefile src/*.o src/qrc_* src/ui_* src/moc_* src/.qmake*

.PHONY: all clean submake

