/*
 * MyCopy.c
 * Author: Christian Würthner
 * Description: Simple copy program.
 */

#include <stdio.h>
#include <inttypes.h>

/* Size of the data blocks copied in bytes */
#define BLOCK_SIZE 1024

int main(int argc, char const *argv[]) {
	/* Check number of arguments and print manual */
	if(argc < 3) {
		printf("ERROR: Too few arguments. Usage: ./MyCopy src dest\n");
		return -3;
	}

	/* Open file pointer for source and handle error */
	FILE *src = fopen(argv[1], "r");
	if(src == NULL) {
		printf("ERROR: Unable to open source file \"%s\"\n", argv[1]);
		return -1;
	}

	/* Open file pointer for destination and handle error */
	FILE *dest = fopen(argv[2], "w+");
	if(dest == NULL) {
		printf("ERROR: Unable to create destination file \"%s\"\n", argv[2]);
		fclose(src);
		return -2;
	}

	/* Create buffer for efficent copying and counters */
	uint8_t buffer[BLOCK_SIZE];
	uint16_t read_count = 0;
	uint64_t copied_count = 0;

	/* Copy blocks */
	while((read_count = fread(buffer, 1, BLOCK_SIZE, src)) > 0) {
		fwrite(buffer, 1, read_count, dest);
		copied_count += read_count;
		printf("%" PRIu64 " bytes copied...\n", copied_count);
	}

	/* Print success and exit */
	printf("SUCCESS.\n");
	return 0;
}