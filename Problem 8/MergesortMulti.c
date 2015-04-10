/*
 * MergesortMulti.c
 * Author: Christian Würthner
 * Description: Merge sort algorithm with multiple threads.
 */

#define SAMPLE_DATA_LENGTH 10000000

#include <stdio.h>
#include <inttypes.h>
#include <time.h>
#include <stdlib.h>
#include <pthread.h>

/* Structure for passing arguments to sort functin */
typedef struct {
	uint8_t *data;
	uint32_t low;
	uint32_t high;
} sort_args_t;

/* Recursive merge sort */
void *sort(void *args_v);

/* Combining two arrays into one sorted */
void merge(uint8_t *data, uint32_t low, uint32_t center, uint32_t high);

/* Checks if value at i is bigger as i+1, therefore checks the success of the sort */
void verify(uint8_t *data, uint32_t length);

int main(int argc, char const *argv[]) {
	/* Init random number generator */
	srand(time(NULL));

	/* Create sample Data */
	uint8_t *data;
	data = (uint8_t*) calloc(SAMPLE_DATA_LENGTH, sizeof(uint8_t));
	for(uint32_t i=0; i<SAMPLE_DATA_LENGTH; i++) {
		data[i] = rand() % 255;
	}

	/* Print status */
	printf("Data generated.\nSorting...\n");

	/* Sort in new thread */
	pthread_t tid;
	sort_args_t args;
	args.data = data;
	args.low = 0;
	args.high = SAMPLE_DATA_LENGTH;
   	pthread_create(&tid, NULL, sort, (void*) &args);

   	/* Join thread */
   	pthread_join(tid, NULL);

	/* Print status */
	printf("Data sorted.\n");

	/* Verfiy result */
	verify(data, SAMPLE_DATA_LENGTH);

	/* Free */
	free(data);
}

void *sort(void *args_v) { 
	sort_args_t args = *(sort_args_t*) args_v;

  	if(args.low < args.high) {
  		/* Find center */
    	uint32_t center = (args.low + args.high) / 2;

    	/* Create args */
    	sort_args_t a[2];

    	a[0].data = args.data;
    	a[0].low = args.low;
    	a[0].high = center;

    	a[1].data = args.data;
    	a[1].low = center + 1;
    	a[1].high = args.high;

    	/* Only use threads if a considerable amount of elements 
    	   must be sorted */
    	if(args.high - args.low > 200000) {
    		/* Create a thread and let it execute the
    		   function recursivly */
    		pthread_t tid;
    		if(pthread_create(&tid, NULL, sort, (void*) a) != 0) {
    			printf("ERROR: Thread creation failed!\n");
    			exit(1);
    		}

    		/* Use the current thread to execute the other half */
    		sort((void*) (a+1));

	    	/* Wait for thread to finish */
	    	pthread_join(tid, NULL);
    	} 

    	/* Not enough elements for parallel computing, do it sequential */
    	else {
    		sort((void*) a);
    		sort((void*) (a+1));

    	}


    	/* Merge them togheter */
    	merge(args.data, args.low, center, args.high);
  	}

  	return NULL;
}

void merge(uint8_t *data, uint32_t low, uint32_t center, uint32_t high) {
	/* Declare needed variables */
  	uint32_t n0, n1, i, j, k;
  	
  	/* Create buffers */
	uint8_t *buf0, *buf1;

  	/* Calc range for the lower and the upper half */
  	n0 = center - low + 1;
  	n1 = high - center;

  	/* Allocate space for buffers in perfect size */
  	buf0 = (uint8_t*) calloc(n0+1, sizeof(data[0]));
  	buf1 = (uint8_t*) calloc(n1+1, sizeof(data[0]));

   	/* Copy data into buffers */
  	for(i=0; i<n0; i++) {
    	buf0[i] = data[low + i];
	}
  
    /* Copy data into buffers */
  	for(j=0; j<n1; j++) {
    	buf1[j]= data[center + j + 1];
 	}
  
  	/* Mark end of buffer */
  	buf0[i] = 255;  
  	buf1[j] = 255;
 
 	/* Reset counters */
  	i=0;
  	j=0;
  
  	/* Merge them by copying always the smaller value back */
  	for(k=low; k<=high; k++) {
    	if(buf0[i] <= buf1[j]) {
      		data[k] = buf0[i++];
    	}
    	else {
      		data[k] = buf1[j++];
    	}
  	}

  	/* Free buffers */
  	free(buf0);
  	free(buf1);
 }

void verify(uint8_t *data, uint32_t length) {
	/* Iterate over data */
	for(uint32_t i=1; i<length; i++) {
		/* Print error and cancel id i-1 is bigger than i */
		if(data[i-1] > data[i]) {
			printf("ERROR: Verification of result failed: Sort error at index %d\n", i);
			return;
		}
	}

	/* If we get here everything is ok */
	printf("SUCESS: Result verfied.\n");
}