BUILD_DIR := ./build

.PHONY: build clean

test: build
	@cd $(BUILD_DIR) && ctest --verbose

build:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake .. && cmake --build .

debug:
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake -DCMAKE_BUILD_TYPE=Debug .. && cmake --build .

clean:
	@rm -rf $(BUILD_DIR)

