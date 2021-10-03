###
#
# SMING Application Makefile for Host (Win32/Linux) platform
#
###

# linker flags used to generate the main object file
LDFLAGS += \
	-m32


# Executable
TARGET_OUT_0			:= $(FW_BASE)/$(APP_NAME)$(TOOL_EXT)

# Hosted Settings
ifneq ($(ENABLE_HOSTED),)
	COMPONENTS_AR := $(USER_LIBDIR)/$(CLIB_PREFIX)Hosted-Lib-$(CMP_Hosted-Lib_LIBHASH).a $(COMPONENTS_AR)
endif

# Target definitions

.PHONY: application
application: $(TARGET_OUT_0)

$(TARGET_OUT_0): $(COMPONENTS_AR)
	$(info $(notdir $(PROJECT_DIR)): Linking $@)
	$(Q) $(LD) $(addprefix -L,$(LIBDIRS)) $(LDFLAGS) -Wl,--start-group $(COMPONENTS_AR) $(addprefix -l,$(LIBS)) -Wl,--end-group -o $@
	$(Q) $(call WriteFirmwareConfigFile,$@)
	$(Q) $(MEMANALYZER) $@ > $(FW_MEMINFO)
	$(Q) cat $(FW_MEMINFO)

##@Tools
.PHONY: valgrind
valgrind: all ##Run the application under valgrind to detect memory issues. Requires `valgrind` to be installed on the host system.
	$(Q) valgrind --track-origins=yes --leak-check=full \
	$(foreach id,$(ENABLE_HOST_UARTID),echo '$(call RunHostTerminal,$(id))' >> $@;) \
	$(TARGET_OUT_0) $(CLI_TARGET_OPTIONS) -- $(HOST_PARAMETERS)


RUN_SCRIPT := $(FW_BASE)/run.sh

.PHONY: run
run: all $(RUN_SCRIPT) ##Run the application image
	$(Q) $(RUN_SCRIPT)

$(RUN_SCRIPT)::
	$(Q) echo '#!/bin/bash' > $@; \
	$(foreach id,$(ENABLE_HOST_UARTID),echo '$(call RunHostTerminal,$(id))' >> $@;) \
	echo '$(TARGET_OUT_0) $(CLI_TARGET_OPTIONS) -- $(HOST_PARAMETERS)' >> $@; \
	chmod a+x $@

##@Flashing

.PHONY: flashconfig
flashconfig: kill_term ##Erase the rBoot config sector
	$(info Erasing rBoot config sector)
	$(call WriteFlash,$(FLASH_RBOOT_ERASE_CONFIG_CHUNKS))
