COMPONENT_LIBNAME :=
COMPONENT_APPCODE := $(COMPONENT_PATH)/appcode

CONFIG_VARS	+= SPIFF_SIZE
ifeq ($(SPI_SIZE), 256K)
	SPIFF_SIZE			?= 131072  #128K
else ifeq ($(SPI_SIZE), 1M)
	SPIFF_SIZE			?= 524288  #512K
else ifeq ($(SPI_SIZE), 2M)
	SPIFF_SIZE			?= 524288  #512K
else ifeq ($(SPI_SIZE), 4M)
	SPIFF_SIZE			?= 524288  #512K
else
	SPIFF_SIZE			?= 196608  #192K
endif


SPIFF_START_ADDR ?= 0x110000


APP_CFLAGS				+= -DSPIFF_SIZE=$(SPIFF_SIZE) -DSPIFF_START_ADDR=$(SPIFF_START_ADDR)
