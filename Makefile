TEST_COMPILER=gcc

BUILD_DIR=./build
UNITY_DIR=./Unity

HEADWATER_ELF=$(BUILD_DIR)/headwater.elf
HEADWATER_HEX=$(HEADWATER_ELF:%.elf=%.hex)

HEADWATER_FLAGS=-g -Wall -fno-strict-aliasing -fno-strict-overflow

HEADWATER_SOURCE_FILES=\
	src/atmega_headwater.c \
	src/atmega_lcd.c \
	src/atmega_spi.c \
	src/api.c \
	src/bytes.c \
	src/debounce.c \
	src/headwater_api.c \
	src/headwater_lcd.c \
	src/headwater_state.c \
	src/headwater_ui.c \
	src/lcd.c \
	src/ui.c

# HEADWATER_AVR_FUSES=-U lfuse:w:0xc2:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m
HEADWATER_AVR_FUSES=-U lfuse:w:0xD7:m
# -U lfuse:w:0xff:m -U hfuse:w:0xd9:m -U efuse:w:0xff:m

TEST_TARGET=$(BUILD_DIR)/run_tests.o

TEST_FLAGS=-g -fno-strict-aliasing -fno-strict-overflow

TEST_SOURCE_FILES=\
  $(UNITY_DIR)/src/unity.c \
  $(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
  src/api.c \
	src/bytes.c \
	src/debounce.c \
	src/lcd.c \
	src/ui.c \
  src/headwater_api.c \
  src/headwater_lcd.c \
  src/headwater_state.c \
  test/test_api.c \
  test/test_bytes.c \
  test/test_debounce.c \
  test/test_lcd.c \
  test/test_ui.c \
  test/test_headwater_api.c \
  test/test_headwater_lcd.c \
  test/test_headwater_state.c \
	test/run_tests.c

TEST_INC_DIRS=-Isrc -I$(UNITY_DIR)/src -I$(UNITY_DIR)/extras/fixture/src

all:
	make clean
	make $(HEADWATER_HEX)

# HEADWATER

flash: $(HEADWATER_HEX)
	sudo avrdude -v -c usbtiny -p atmega328p $(HEADWATER_AVR_FUSES) -U flash:w:$(HEADWATER_HEX)

$(HEADWATER_HEX): $(HEADWATER_ELF)
	avr-objcopy -j .text -j .data -O ihex $< $@

$(HEADWATER_ELF):
	avr-gcc $(HEADWATER_FLAGS) -Os -mmcu=atmega328p -std=c11 -o $@ $(HEADWATER_SOURCE_FILES)

test: $(TEST_TARGET)

$(TEST_TARGET): clean
	$(TEST_COMPILER) $(TEST_FLAGS) $(TEST_INC_DIRS) $(TEST_SOURCE_FILES) -o $(TEST_TARGET)
	$(TEST_TARGET) -v

clean:
	rm $(BUILD_DIR)/* || true

term:
	# m 5 4 1 2 1 2 2
	picocom -b115200 /dev/ttyUSB0

analyzer:
	pulseview

size:
	avr-size -Cx --mcu atmega328p $(HEADWATER_ELF)
