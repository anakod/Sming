COMPONENT_DEPENDS := Adafruit_GFX

COMPONENT_RELINK_VARS := TFT_CS_PIN TFT_DC_PIN TFT_RESET_PIN

ifeq ($(SMING_ARCH),Esp32)
TFT_CS_PIN		?= 18
TFT_DC_PIN		?= 19
TFT_RESET_PIN	?= 21
else
TFT_CS_PIN		?= 15
TFT_DC_PIN		?= 5
TFT_RESET_PIN	?= 4
endif

COMPONENT_CXXFLAGS += -DTFT_CS_PIN=$(TFT_CS_PIN) -DTFT_DC_PIN=$(TFT_DC_PIN)

# Reset pin is optional
ifdef TFT_RESET_PIN
COMPONENT_CXXFLAGS += -DTFT_RESET_PIN=$(TFT_RESET_PIN)
endif
