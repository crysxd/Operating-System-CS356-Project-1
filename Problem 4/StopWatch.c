/*
 * StopWatch.c
 * Author: Christian Würthner
 * Description: Call all copy programs and stop their execution times
 */

#define SAMPLE_FILE_NAME "StopWatchSample"
#define SAMPLE_FILE_COPY_NAME "StopWatchSampleCopy"
#define COPY_PROGRAMS {"cp", "./MyCopy", "./ForkCopy", "./PipeCopy"}
#define SAMPLE_FILE_BLOCK_SIZE 1024 /* 1 KiB */
#define SAMPLE_FILE_SIZE SAMPLE_FILE_BLOCK_SIZE * 1024 * 16 /* 16 MiB */
#define EXECLP_ERROR 42
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

/* Struct for all necessary data for measuring the elapsed time with both methods */
typedef struct {
 	char* program_name;
 	double start_times[3];
 	double end_times[3];
 	double elapsed_ms[3];
} stopwatch_t;

/* Creates a sample file with the size defined in SAMPLE_FILE_SIZE */
void create_sample_file();

/* Deletes the sample file and it's copy */
void clean_up();

/* Calls the copy program specified by copy_program_name */
int call_copy(char* copy_program_name);

/* Deletes the file defined by name */
void delete_file(char* name);

/* Sets the start fields in time to the current time */
void start_stopwatch(stopwatch_t *time);

/* Sets the stop fields in time to the current time and calculates the elapsed time */
void stop_stopwatch(stopwatch_t *time);

/* Sets the current times in the given double array */
void set_times(double *times);

int main(int argc, char const *argv[]) {
 	/* create a sample file to test the copy processes */
 	create_sample_file();

 	/* Create necessary fields */
 	char *programs[] = COPY_PROGRAMS;
 	uint8_t program_count = sizeof(programs)/sizeof(char*);
 	stopwatch_t *times = malloc(sizeof(stopwatch_t) * program_count);
 	int status;

 	/* Iterate over all processes to test */
 	for(uint16_t i=0; i<program_count; i++) {
 		/* Set program name */
 		times[i].program_name = programs[i];

 		/* Save start time */
 		start_stopwatch(times + i);

 		/* Copy and wait */
 		status = call_copy(programs[i]);

		/* Query the plain exit status */
		status = WEXITSTATUS(status);

 		/* Check success */
 		if(status != 0) {
 			printf("ERROR: Child process finished abnormally with status %d\n", status);

 			if(status == EXECLP_ERROR) {
 				printf("HINT: execlp() failed. Please make sure that you call PipeCopy in the bin folder and all needed programs are also in the bin folder.\n");
 			}

 			/* cleanup the files */
 			clean_up();

 			return 2;
 		}

 		/* Calculated time elapsed */
 		stop_stopwatch(times + i);
 	}

 	/* clean up all files created */
 	clean_up();

 	/* Print results */
 	printf("RESULT:\n");
 	printf("==================================================================================\n");
 	printf("| %-14s | clock()\t | gettimeofday()\t | clock_gettime()\t |\n", "Program");
 	printf("----------------------------------------------------------------------------------\n");
 	for(uint8_t i=0; i<program_count; i++) {
 		printf("| %-14s | %.3fms\t | %.12fms\t | %.12fms\t |\n", 
 			times[i].program_name, times[i].elapsed_ms[0], times[i].elapsed_ms[1], times[i].elapsed_ms[2]);
 	}
 	printf("==================================================================================\n");

 	return 0;
 }

void create_sample_file() {
	/* Create sample file and catch error */
	FILE *sample = fopen(SAMPLE_FILE_NAME, "w+");
	if(sample == NULL) {
		printf("ERROR: Unable to create sample file '%s'\n", SAMPLE_FILE_NAME);
		exit(1);
	}

	/* Create a sample buffer filled with 42s */
	uint8_t buffer[SAMPLE_FILE_BLOCK_SIZE] = {42};

	/* Fill sample file */
	for(uint64_t i=0; i<SAMPLE_FILE_SIZE; i+=SAMPLE_FILE_BLOCK_SIZE) {
		fwrite(buffer, 1, SAMPLE_FILE_BLOCK_SIZE, sample);
	}

	/* Close sample file */
	fclose(sample);

	/* Print success */
	printf("SUCCESS: Created sample file '%s' with a total size of %d bytes\n", SAMPLE_FILE_NAME, SAMPLE_FILE_SIZE);
}

void clean_up() {
	/* Delete both files and print success */
	delete_file(SAMPLE_FILE_NAME);
	delete_file(SAMPLE_FILE_COPY_NAME);
	printf("SUCCESS: Cleanup complete\n");
}

void delete_file(char* name) {
	/* Delete file and handle error */
	if(remove(name) != 0) {
		printf("ERROR: Unable to delete file '%s' (%s)\n", name, strerror(errno));
		return;
	}

	/* Print success */
	printf("SUCCESS: Deleted file '%s'\n", name);
}

int call_copy(char* copy_program_name) {
	printf("CALL: %s %s %s\n", copy_program_name, SAMPLE_FILE_NAME, SAMPLE_FILE_COPY_NAME);

	/* Fork process */
	pid_t pid = fork();

	/* Parent code */
	if(pid > 0) {
		/* Wait and return status */
		int status;
		wait(&status);
		return status;
	}

	/* Child code */
	if(pid == 0) {
		/* Redirect output to /dev/null/ */
		int devNull = open("/dev/null", O_WRONLY);
		dup2(devNull, 1);
		dup2(devNull, 0);

		/* exec */
		execlp(copy_program_name, copy_program_name, SAMPLE_FILE_NAME, SAMPLE_FILE_COPY_NAME, NULL);

		/* If this code is executed, execlp failed. */
		exit(EXECLP_ERROR);
	}

	/* Error handling */
	if(pid < 0) {
		printf("ERROR: Unable to fork process!\n");
		exit(2);
	}

	return 0;
}

void start_stopwatch(stopwatch_t *time) {
	/* Set start times */
	set_times(time->start_times);
}

void stop_stopwatch(stopwatch_t *time) {
	/* Set end times */
	set_times(time->end_times);

	/* Calculate elapsed time */
	for(uint8_t i=0; i<sizeof(time->elapsed_ms); i++) {
		time->elapsed_ms[i] = time->end_times[i] - time->start_times[i];
	}
}

void set_times(double *times) {
	/* Set start time for clock() */
	times[0] = (double) clock() / CLOCKS_PER_SEC / 1000.;

	/* Set start time for gettimeofday() */
	struct timeval tv;
	gettimeofday(&tv, NULL); 
	times[1] = tv.tv_sec * 1000. + tv.tv_usec / 1000.;

	/* Set start time for clock_gettime() */
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME, &ts);
	times[2] = ts.tv_sec * 1000. + ts.tv_nsec / 1000000.;
}