# Options to add for configuring host network behaviour
CACHE_VARS				+= HOST_NETWORK_OPTIONS
HOST_NETWORK_OPTIONS	?=

COMPONENT_VARS		:= ENABLE_WPS
ifeq ($(ENABLE_WPS),1)
   GLOBAL_CFLAGS	+= -DENABLE_WPS=1
   EXTRA_LIBS		:= wps
endif
