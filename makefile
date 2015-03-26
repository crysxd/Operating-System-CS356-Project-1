all: directories MyCopy complete

clean:
	rm -r bin

directories:
	@mkdir -p bin
	@echo "Output directories created"

MyCopy:
	@gcc "Problem 1/MyCopy.c" -o bin/MyCopy
	@echo "Build MyCopy {Problem 1}"

complete:
	@echo "All binaries build to directory 'bin/'"