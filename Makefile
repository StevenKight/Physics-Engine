.PHONY: all build test package clean help

all: build

## Build the project (configure + compile)
build:
	cmake -B build
	cmake --build build

## Run the test suite
test: build
	cd build && ctest --output-on-failure

## Package the Blender addon into physics_engine.zip
package:
	python3 scripts/package_blender.py

## Remove all build artifacts and generated zip
clean:
	rm -rf build physics_engine.zip

## Show available targets
help:
	@echo "Usage: make [target]"
	@echo ""
	@echo "  build    Configure and compile the project"
	@echo "  test     Build then run the test suite"
	@echo "  package  Build the Blender addon zip (physics_engine.zip)"
	@echo "  clean    Remove build/ and physics_engine.zip"
	@echo "  help     Show this message"
