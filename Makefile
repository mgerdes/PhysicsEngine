BUILD_DIR = ./build
BIN = sim
HEADERS = $(wildcard src/*.h)
SOURCES = $(wildcard src/*.cpp)
OBJS = $(SOURCES:%.cpp=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:%.o=%.d)
CFLAGS = -Iobjects -Isrc -lGL -lGLEW -lglfw -lm -O2

all: $(BUILD_DIR) $(BUILD_DIR)/$(BIN)

$(BUILD_DIR):
	mkdir ${BUILD_DIR}
	mkdir ${BUILD_DIR}/src

$(BUILD_DIR)/%.o: %.cpp
	g++ $< $(CFLAGS) -c -MMD -o $@

$(BUILD_DIR)/$(BIN): $(OBJS)
	g++ -o $@ $(OBJS) $(CFLAGS)

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
