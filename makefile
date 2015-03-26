CC     = /usr/bin/gcc
CFLAGS = -Wall -g

all: directories MyCopy ForkCopy complete

clean:
	rm -r bin

directories:
	@mkdir -p bin
	@echo "Output directories created"

MyCopy:
	@gcc "Problem 1/MyCopy.c" -o bin/MyCopy
	@echo "Build MyCopy {Problem 1}"

ForkCopy:
	@gcc "Problem 2/ForkCopy.c" -o bin/ForkCopy
	@echo "Build ForkCopy {Problem 2}"

complete:
	@echo "All binaries build to directory 'bin/'"