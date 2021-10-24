COMPONENT_SUBMODULES := picotool
PICOTOOL_SRC	:= $(COMPONENT_PATH)/picotool
LIBUSB_PATH		:= $(COMPONENT_PATH)/libusb
LIBUSB_DLL		:= libusb-1.0.dll
PICOTOOL		:= $(TOOLS_BASE)/picotool$(TOOL_EXT)

DEBUG_VARS		+= PICOTOOL

CMAKE_OPTIONS :=

ifeq ($(UNAME),Windows)
PICOTOOL_CMAKE_OPTIONS += \
	-DLIBUSB_INCLUDE_DIR=$(LIBUSB_PATH) \
	-DLIBUSB_LIBRARIES=$(LIBUSB_PATH)/$(LIBUSB_DLL) \
	-G "MSYS Makefiles"

COMPONENT_TARGETS += $(TOOLS_BASE)/$(LIBUSB_DLL)

$(COMPONENT_RULE)$(TOOLS_BASE)/$(LIBUSB_DLL): $(LIBUSB_PATH)/$(LIBUSB_DLL)
	cp $< $@
endif

COMPONENT_TARGETS += $(PICOTOOL)

$(COMPONENT_RULE)$(PICOTOOL):
	$(Q) mkdir -p $(@D)
	$(Q) cd $(@D) && $(CMAKE) $(PICOTOOL_CMAKE_OPTIONS) $(PICOTOOL_SRC) && $(MAKE)


# Read flash manufacturer ID and determine actual size
define ReadFlashID
	$(info ReadFlashID)
	$(Q) $(PICOTOOL) info -a $(TARGET_BIN)
	# $(PICOTOOL) help info
 endef