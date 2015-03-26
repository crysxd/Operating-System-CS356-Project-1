CC     = @/usr/bin/gcc
CFLAGS = -Wall -g -std=c99
ECHO   = @echo

all: directories MyCopy PipeCopy complete

clean:
	@rm -rf bin
	$(ECHO) "All binaries removed"

directories:
	@mkdir -p bin
	$(ECHO) "Output directories created"

MyCopy:
	$(CC) $(CFLAGS) "Problem 1/MyCopy.c" -o bin/MyCopy
	$(ECHO) "Build MyCopy {Problem 1}"

PipeCopy:
	$(CC) $(CFLAGS) "Problem 3/PipeCopy.c" -o bin/PipeCopy
	$(ECHO) "Build PipeCopy {Problem 3}"

complete:
	$(ECHO) "All binaries build to directory 'bin/'"