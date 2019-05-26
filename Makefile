.PHONY: analyzer clean docs flash loc size term test

TEST_CC ?= gcc-4.9

BUILD_DIR=./build
UNITY_DIR=./Unity

HEADWATER_ELF=$(BUILD_DIR)/headwater.elf
HEADWATER_HEX=$(HEADWATER_ELF:%.elf=%.hex)

HEADWATER_FLAGS=-std=c11 -g -Os -mmcu=atmega328p -Ilib -Wall -Wpedantic -Wdouble-promotion -Wshadow -Wlogical-op -fno-strict-aliasing -fno-strict-overflow -fno-strict-aliasing -fno-strict-overflow

HEADWATER_SOURCE_FILES=\
	lib/api.c \
	lib/bytes.c \
	lib/debounce.c \
	lib/lcd.c \
	lib/midi.c \
	lib/queue.c \
	lib/ui.c \
	src/atmega_eeprom.c \
	src/atmega_io.c \
	src/atmega_lcd.c \
	src/atmega_spi.c \
	src/atmega_uart.c \
	src/headwater_api.c \
	src/headwater_state.c \
	src/headwater_ui.c \
	src/main.c

HEADWATER_AVR_FUSES=-U lfuse:w:0xD7:m -U hfuse:w:0xD1:m

TEST_TARGET=$(BUILD_DIR)/run_tests.o

TEST_FLAGS=-std=c11 -g -fno-strict-aliasing -fno-strict-overflow

TEST_SOURCE_FILES=\
  $(UNITY_DIR)/src/unity.c \
  $(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
  lib/api.c \
	lib/bytes.c \
	lib/debounce.c \
	lib/lcd.c \
	lib/midi.c \
	lib/queue.c \
	lib/ui.c \
  src/headwater_api.c \
  src/headwater_state.c \
  src/headwater_ui.c \
  test/test_api.c \
  test/test_bytes.c \
  test/test_debounce.c \
  test/test_lcd.c \
  test/test_midi.c \
  test/test_queue.c \
  test/test_ui.c \
  test/test_headwater_api.c \
  test/test_headwater_state.c \
  test/test_headwater_ui.c \
	test/run_tests.c

TEST_INC_DIRS=-Isrc -Ilib -I$(UNITY_DIR)/src -I$(UNITY_DIR)/extras/fixture/src

all:
	make clean
	make $(HEADWATER_HEX)

flash: $(HEADWATER_HEX)
	sudo avrdude -v -c usbtiny -p atmega328p $(HEADWATER_AVR_FUSES) -U flash:w:$(HEADWATER_HEX)

$(HEADWATER_HEX): $(HEADWATER_ELF)
	avr-objcopy -j .text -j .data -O ihex $< $@

$(HEADWATER_ELF):
	avr-gcc $(HEADWATER_FLAGS) -o $@ $(HEADWATER_SOURCE_FILES)

test: $(TEST_TARGET)

$(TEST_TARGET): clean
	$(TEST_CC) $(TEST_FLAGS) $(TEST_INC_DIRS) $(TEST_SOURCE_FILES) -o $(TEST_TARGET)
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

loc:
	cloc test
	cloc lib src

docs:
	doxygen
	/opt/google/chrome/chrome docs/html/index.html
