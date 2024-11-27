# pico_mem_ops

RUNTIME_INIT_FUNC += \
	__aeabi_mem_init

ifeq ($(SMING_SOC),rp2040)
WRAPPED_FUNCTIONS += \
	memcpy \
	memset \
	__aeabi_memcpy \
	__aeabi_memset \
	__aeabi_memcpy4 \
	__aeabi_memset4 \
	__aeabi_memcpy8 \
	__aeabi_memset8
endif
