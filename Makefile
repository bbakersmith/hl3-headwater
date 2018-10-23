TEST_COMPILER=gcc

BUILD_DIR=./build
UNITY_DIR=./Unity

TARGET_ELF=$(BUILD_DIR)/headwater_atmega.elf
TARGET_HEX=$(TARGET_ELF:%.elf=%.hex)

SOURCE_FILES=\
  src/headwater.c \
	src/headwater_atmega.c

TEST_TARGET = $(BUILD_DIR)/run_tests.o

TEST_SOURCE_FILES=\
  $(UNITY_DIR)/src/unity.c \
  $(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
  src/headwater.c \
  test/test_headwater.c \
	test/run_tests.c

TEST_INC_DIRS=-Isrc -I$(UNITY_DIR)/src -I$(UNITY_DIR)/extras/fixture/src

all:
	make clean
	make $(TARGET_HEX)

flash: all
	sudo avrdude -c usbtiny -p atmega328p -U flash:w:$(TARGET_HEX)

$(TARGET_HEX): $(TARGET_ELF)
	avr-objcopy -j .text -j .data -O ihex $< $@

$(TARGET_ELF):
	avr-gcc -g -Os -mmcu=atmega328p -o $@ $(SOURCE_FILES)

test: $(TEST_TARGET)

$(TEST_TARGET): clean
	$(TEST_COMPILER) $(TEST_INC_DIRS) $(TEST_SOURCE_FILES) -o $(TEST_TARGET)
	$(TEST_TARGET) -v

clean:
	rm $(BUILD_DIR)/* || true
