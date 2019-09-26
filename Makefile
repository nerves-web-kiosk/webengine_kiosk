PREFIX = $(MIX_APP_PATH)/priv
BUILD  = $(MIX_APP_PATH)/obj
SRC_DIR = $(dir $(realpath $(firstword $(MAKEFILE_LIST))))

# Override if using a specific Qt toolkit version
QMAKE ?= qmake

calling_from_make:
	mix compile

all: copy_assets submake

copy_assets: $(PREFIX)
	cp -r assets/www $(PREFIX)

$(PREFIX) $(BUILD):
	mkdir -p $@

$(BUILD)/Makefile: $(BUILD) src/kiosk.pro
	cd $(BUILD) && $(QMAKE) $(SRC_DIR)/src/kiosk.pro

submake: $(BUILD)/Makefile
	+$(MAKE) -j3 -C $(BUILD)
	+INSTALL_ROOT="$(MIX_APP_PATH)" $(MAKE) -C $(BUILD) install

ifeq ($(MIX_APP_PATH),)
clean:
	mix clean
else
clean:
	$(RM) -r $(PREFIX) $(BUILD)
endif

.PHONY: all clean submake copy_assets

