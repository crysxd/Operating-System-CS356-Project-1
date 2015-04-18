CC     = @/usr/bin/gcc
CFLAGS = -Wall -g -std=c99
ECHO   = @echo

all: directories MyCopy ForkCopy PipeCopy StopWatch MyShell MoreShell DupShell Mergesort BurgerBuddies complete

clean:
	@rm -rf bin
	$(ECHO) "All binaries removed"

directories:
	@mkdir -p bin
	$(ECHO) "Output directories created"

MyCopy: directories
	$(CC) $(CFLAGS) "Problem 1/MyCopy.c" -o bin/MyCopy
	$(ECHO) "Build MyCopy {Problem 1}"

ForkCopy: directories MyCopy
	$(CC) $(CFLAGS) "Problem 2/ForkCopy.c" -o bin/ForkCopy
	$(ECHO) "Build ForkCopy {Problem 2}"

PipeCopy: directories
	$(CC) $(CFLAGS) "Problem 3/PipeCopy.c" -o bin/PipeCopy
	$(ECHO) "Build PipeCopy {Problem 3}"

StopWatch: directories MyCopy ForkCopy PipeCopy
	$(CC) $(CFLAGS) "Problem 4/StopWatch.c" -o bin/StopWatch
	$(ECHO) "Build StopWatch {Problem 4}"

MyShell: directories
	$(CC) $(CFLAGS) "Problem 5/MyShell.c" -o bin/MyShell
	$(ECHO) "Build MyShell {Problem 5}"

MoreShell: directories
	$(CC) $(CFLAGS) "Problem 6/MoreShell.c" -o bin/MoreShell
	$(ECHO) "Build MoreShell {Problem 6}"

DupShell: directories
	$(CC) $(CFLAGS) "Problem 7/DupShell.c" -o bin/DupShell
	$(ECHO) "Build DupShell {Problem 7}"

Mergesort: directories
	$(CC) $(CFLAGS) "Problem 8/MergesortSingle.c" -o bin/MergesortSingle
	$(CC) $(CFLAGS) "Problem 8/MergesortMulti.c" -lpthread -o bin/MergesortMulti
	$(ECHO) "Build Mergesort {Problem 8}"

BurgerBuddies: directories
	$(CC) $(CFLAGS) "Problem 9/BurgerBuddies.c" -lpthread -o bin/BBC
	$(ECHO) "Build BurgerBuddies {Problem 9}"

complete:
	$(ECHO) "All binaries build to directory 'bin/'"
