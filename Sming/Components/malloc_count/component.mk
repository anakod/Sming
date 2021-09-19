COMPONENT_RELINK_VARS += ENABLE_MALLOC_COUNT

ENABLE_MALLOC_COUNT ?= 1

ifeq ($(ENABLE_MALLOC_COUNT),1)

COMPONENT_CXXFLAGS += -DENABLE_MALLOC_COUNT=1

# Hook all the memory allocation functions we need to monitor heap activity
MC_WRAP_FUNCS := \
	malloc \
	calloc \
	realloc \
	free

ifeq ($(SMING_ARCH),Esp8266)
MC_WRAP_FUNCS += \
	pvPortMalloc \
	pvPortCalloc \
	pvPortRealloc \
	pvPortZalloc \
	pvPortZallocIram \
	vPortFree
else
MC_WRAP_FUNCS += \
	strdup
endif

EXTRA_LDFLAGS := $(call Wrap,$(MC_WRAP_FUNCS))

endif
