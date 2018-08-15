# Override if using a specific Qt toolkit version
QMAKE ?= qmake

all: submake

src/Makefile:
	cd src && $(QMAKE) kiosk.pro

submake: src/Makefile
	+$(MAKE) -j3 -C src install

clean:
	-rm -fr priv/kiosk src/kiosk src/Makefile src/*.o src/qrc_* src/ui_* src/moc_* src/.qmake*

.PHONY: all clean submake

