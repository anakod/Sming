COMPONENT_LIBNAME	:=

# spiffs_get_storage_config()
COMPONENT_APPCODE	:= $(ESP8266_COMPONENTS)/rboot/appcode
RBOOT_SPIFFS_0		?= 0x100000
RBOOT_SPIFFS_1		?= 0x300000
APP_CFLAGS			+= -DRBOOT_SPIFFS_0=$(RBOOT_SPIFFS_0)
APP_CFLAGS			+= -DRBOOT_SPIFFS_1=$(RBOOT_SPIFFS_1)

# Use DD to update FLASH_BIN, which represents the entire flash device contents

DD					:= dd

CACHE_VARS			+= FLASH_BIN
FLASH_BIN			?= $(FW_BASE)/flash.bin

CONFIG_VARS			+= SPI_SIZE
SPI_SIZE			?= 4M

# Write data to flash
# $1 -> Start offset
# $2 -> File containing data to write
define WriteFlashChunk
	$(info WriteFlash $1 -> $2)
	$(Q) if [ ! -f $(FLASH_BIN) ]; then \
		$(EraseFlash); \
	fi
	$(Q) $(DD) if=$2 of=$(FLASH_BIN) obs=1 seek=$$(($1)) conv=notrunc
endef

# Write one or more chunks to flash
# $1 -> List of `Offset=File` chunks
define WriteFlash
	$(foreach c,$1,$(call WriteFlashChunk,$(word 1,$(subst =, ,$c)),$(word 2,$(subst =, ,$c))))
endef

# Reset/create flash backing file
define EraseFlash
	$(DD) if=/dev/zero ibs=1 count=$(SPI_SIZE) | tr "\000" "\377" > $(FLASH_BIN)
endef 
