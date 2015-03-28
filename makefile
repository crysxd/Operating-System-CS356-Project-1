CC     = @/usr/bin/gcc
CFLAGS = -Wall -g -std=c99
ECHO   = @echo

all: directories MyCopy ForkCopy PipeCopy StopWatch MyShell MoreShell  complete

clean:
	@rm -rf bin
	$(ECHO) "All binaries removed"

directories:
	@mkdir -p bin
	$(ECHO) "Output directories created"

MyCopy:
	$(CC) $(CFLAGS) "Problem 1/MyCopy.c" -o bin/MyCopy
	$(ECHO) "Build MyCopy {Problem 1}"

ForkCopy:
	$(CC) $(CFLAGS) "Problem 2/ForkCopy.c" -o bin/ForkCopy
	$(ECHO) "Build ForkCopy {Problem 2}"

PipeCopy:
	$(CC) $(CFLAGS) "Problem 3/PipeCopy.c" -o bin/PipeCopy
	$(ECHO) "Build PipeCopy {Problem 3}"

StopWatch:
	$(CC) $(CFLAGS) "Problem 4/StopWatch.c" -o bin/StopWatch
	$(ECHO) "Build StopWatch {Problem 4}"

MyShell:
	$(CC) $(CFLAGS) "Problem 5/MyShell.c" -o bin/MyShell
	$(ECHO) "Build MyShell {Problem 5}"

MoreShell:
	$(CC) $(CFLAGS) "Problem 6/MoreShell.c" -o bin/MoreShell
	$(ECHO) "Build MoreShell {Problem 6}"

complete:
	$(ECHO) "All binaries build to directory 'bin/'"
