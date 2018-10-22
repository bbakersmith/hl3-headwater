C_COMPILER=gcc

BUILD_DIR=./build
UNITY_DIR=./Unity

CFLAGS=

TEST_TARGET = $(BUILD_DIR)/run_tests.o

TEST_SOURCE_FILES=\
  $(UNITY_DIR)/src/unity.c \
  $(UNITY_DIR)/extras/fixture/src/unity_fixture.c \
  src/headwater.c \
  test/test_headwater.c \
	test/run_tests.c

INC_DIRS=-Isrc -I$(UNITY_DIR)/src -I$(UNITY_DIR)/extras/fixture/src

all: clean $(TEST_TARGET)

$(TEST_TARGET):
	$(C_COMPILER) $(CFLAGS) $(INC_DIRS) $(TEST_SOURCE_FILES) -o $(TEST_TARGET)
	- $(TEST_TARGET) -v

clean:
	rm $(TEST_TARGET) || true
