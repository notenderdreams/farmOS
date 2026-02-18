BUILD_DIR := build
BINARY := $(BUILD_DIR)/src/app/farmos

.PHONY: build test run clean

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR)

test: build
	cd $(BUILD_DIR) && ctest --output-on-failure

run: build
	./$(BINARY)

clean:
	rm -rf $(BUILD_DIR)
