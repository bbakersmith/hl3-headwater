TEST_COMPILER=gcc

BUILD_DIR=./build
UNITY_DIR=./Unity

TARGET_ELF=$(BUILD_DIR)/headwater.elf
TARGET_HEX=$(TARGET_ELF:%.elf=%.hex)

SOURCE_FILES=\
	src/atmega_headwater.c \
	src/atmega_spi.c \
  src/headwater_state.c \
  src/spi.c

AVR_FUSES=-U lfuse:w:0xD7:m

TEST_TARGET = $(BUILD_DIR)/run_tests.o

TEST_SOURCE_FILES=\
  $(UNITY_DIR)/src/unity.c \
  $(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
  src/api.c \
  src/headwater_state.c \
	src/spi.c \
  test/test_api.c \
  test/test_headwater_state.c \
  test/test_spi.c \
	test/run_tests.c

TEST_INC_DIRS=-Isrc -I$(UNITY_DIR)/src -I$(UNITY_DIR)/extras/fixture/src

all:
	make clean
	make $(TARGET_HEX)

flash: all
	sudo avrdude -v -c usbtiny -p atmega328p $(AVR_FUSES) -U flash:w:$(TARGET_HEX)

$(TARGET_HEX): $(TARGET_ELF)
	avr-objcopy -j .text -j .data -O ihex $< $@

$(TARGET_ELF):
	avr-gcc -g -Os -mmcu=atmega328p -std=c99 -o $@ $(SOURCE_FILES)

test: $(TEST_TARGET)

$(TEST_TARGET): clean
	$(TEST_COMPILER) -g $(TEST_INC_DIRS) $(TEST_SOURCE_FILES) -o $(TEST_TARGET)
	$(TEST_TARGET) -v

clean:
	rm $(BUILD_DIR)/* || true

term:
	# m 5 4 1 2 1 2 2
	picocom -b115200 /dev/ttyUSB0
