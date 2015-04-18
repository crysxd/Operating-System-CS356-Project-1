/*
 * BurgerBuddies.c
 * Author: Christian Würthner
 * Description: Burger buddies problem.
 */

#define COOK_COUNT 			3
#define CASHIER_COUNT		2
#define CUSTOMER_COUNT		4
#define RACK_HOLDER_SIZE	4
#define WAITING_TIME		5

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>

/* Flag which will be set to true when the threads should terminate themselfs */
bool interrupt = false;

/* define struct representing a cashier */
typedef struct {
	uint8_t id;
	sem_t *order;
	sem_t *burger;
} cashier_t;

/* define struct to pass args to the run functions */
typedef struct {
	uint8_t id;
	sem_t *init_done;
} simple_arg_t;

/* Declare funcitons for the cook, cashiers and customers to run */
void *cook_run();
void *cashier_run();
void *customer_run();

/* Declare function to check current system state */
void assure_state();

/* Define all needed semaphores */
sem_t rack;
sem_t cook;
sem_t cashier;
sem_t cashier_awake;
sem_t customer;
sem_t customer_private_mutex;

/* Define memory space to store a cashier */
cashier_t cashier_exchange;

/* Define the counter for available burgers in the rack */
uint8_t burger_count = 0;

int main(int argc, char **argv) {
	/* Init random number generator */
	srand(time(NULL));

	/* Init all semaphores */
	sem_init(&rack, 0, 1);
	sem_init(&cashier, 0, 1);
	sem_init(&cashier_awake, 0, 0);
	sem_init(&cook, 0, RACK_HOLDER_SIZE);
	sem_init(&customer, 0, 0);
	sem_init(&customer_private_mutex, 0, 1);

	/* Create semaphore to synchronise thread init and args */
	simple_arg_t args;
	sem_t init_done;
	sem_init(&init_done, 0, 0);
	args.init_done = &init_done;

	/* Start all cook threads */
	pthread_t cooks[COOK_COUNT];
	for(uint8_t i=0; i<COOK_COUNT; i++) {
		/* Set id for cook */
		args.id = i;

		/* Start cook thread and pass args, handle possible erros */
		if(pthread_create(cooks+i, NULL, cook_run, (void*) &args)) {
			printf("[MAIN]\t\t ERROR: Unable to create cook thread.\n");
			exit(1);
		}

		/* Wait until the cook is initialised and ready to run */
		sem_wait(&init_done);
	}

	/* Start all cashier threads */
	pthread_t cashiers[CASHIER_COUNT];
	for(uint8_t i=0; i<CASHIER_COUNT; i++) {
		/* Set id for cashier */
		args.id = i;

		/* Start cashier thread and pass args, handle possible erros */
		if(pthread_create(cashiers+i, NULL, cashier_run, (void*) &args)) {
			printf("[MAIN]\t\t ERROR: Unable to create cashier thread.\n");
			exit(2);
		}

		/* Wait until the cashier is initialised and ready to run */
		sem_wait(&init_done);
	}

	/* Start all customer threads */
	pthread_t customers[CUSTOMER_COUNT];
	for(uint8_t i=0; i<CUSTOMER_COUNT; i++) {
		/* Set id for customer */
		args.id = i;

		/* Start customer thread and pass args, handle possible erros */
		if(pthread_create(customers+i, NULL, customer_run, (void*) &args)) {
			printf("[MAIN]\t\t ERROR: Unable to create customer thread.\n");
			exit(3);
		}

		/* Wait until the customer is initialised and ready to run */
		sem_wait(&init_done);
	}

	/* destroy init semaphore */
	sem_destroy(&init_done);

	/* wait for all customer threads to finish */
	for(uint8_t i=0; i<CUSTOMER_COUNT; i++) {
		/* Join customer and handle possible errors */
		if(pthread_join(customers[i], NULL)) {
			printf("[MAIN]\t\t ERROR: Unable to join cutomers[%d]\n", i);
			exit(4);
		}
	}

	/*
	 * CLEANUP
	 */

	printf("[MAIN]\t\t SUCCESS: All customers terminated\n");
	printf("\n----------------\n\n[MAIN]\t\t SUCCESS: Starting Cleanup\n");

	/* Set interrupt flag */
	interrupt = true;

	/* Wake all cooks up, they will see the interrupt flag and will exit */
	for(uint8_t i=0; i<COOK_COUNT; i++) {
		sem_post(&cook);
	}

	/* Wake all cashiers up, they will see the interrupt flag and will exit */
	for(uint8_t i=0; i<CASHIER_COUNT; i++) {
		sem_post(&customer);
	}

	/* All threads were woken up */
	printf("[MAIN]\t\t SUCCESS: Told all threads to terminate themselves\n");

	/* wait for all cook threads to finish */
	for(uint8_t i=0; i<COOK_COUNT; i++) {
		/* Join customer and handle possible errors */
		if(pthread_join(cooks[i], NULL)) {
			printf("[MAIN]\t\t ERROR: Unable to join cooks[%d]\n", i);
			exit(5);
		}
	}

	/* wait for all cashier threads to finish */
	for(uint8_t i=0; i<CASHIER_COUNT; i++) {
		/* Join customer and handle possible errors */
		if(pthread_join(cashiers[i], NULL)) {
			printf("[MAIN]\t\t ERROR: Unable to join cashiers[%d]\n", i);
			exit(6);
		}
	}

	/* Print status and exit */
	assure_state();
	printf("[MAIN]\t\t SUCCESS: All threads terminated, state consistent.\n");
}

void *cook_run(void *args) {
	/* Get args from void pointer */
	simple_arg_t *args_ptr = (simple_arg_t*) args;

	/* Get id */
	uint8_t cook_id = args_ptr->id;

	/* Print status and signal the init_done semaphore */
	printf("[COOK %d]\t CREATED.\n", cook_id);
	sem_post(args_ptr->init_done);

	/* Infinite loop */
	while(1) {
		/* Aquire cook semaphore */
		sem_wait(&cook);

		/* Check if we should terminate */
		if(interrupt) {
			break;
		}

		/* Cook */
		sleep(rand() % WAITING_TIME);

		/* Lock rack and produce burger */
		sem_wait(&rack);
		assure_state();
		burger_count++;
		assure_state();
		sem_post(&rack);

		printf("[COOK %d]\t Placed new burger in rack.\n", cook_id);

		/* Signal a waiting cashier */
		sem_post(&cashier);
	}

	printf("[COOK %d]\t DONE.\n", cook_id);
	return NULL;
}

void *cashier_run(void *args) {
	/* Get args from void pointer */
	simple_arg_t *args_ptr = (simple_arg_t*) args;

	/* Get id */
	uint8_t cashier_id = args_ptr->id;

	/* Create private order and burger semaphore */
	sem_t order;
	sem_t burger;
	sem_init(&order, 0, 0);
	sem_init(&burger, 0, 0);

	/* Print status and signal the init_done semaphore */
	printf("[CASHIER %d]\t CREATED.\n", cashier_id);
	sem_post(args_ptr->init_done);

	/* Infinite loop */
	while(1) {
		 /* Wait for customer */
		sem_wait(&customer);

		/* Check if we should terminate */
		if(interrupt) {
			break;
		}

		/* print status */
		printf("[CASHIER %d]\t Serving customer.\n", cashier_id);

		/* Save my id to the exchange */
		cashier_exchange.order = &order;
		cashier_exchange.burger = &burger;
		cashier_exchange.id = cashier_id;

		/* Tell customer that I am awake and information is placed */
		sem_post(&cashier_awake);

		/* Wait for the order and print sttaus */
		sem_wait(&order);
		printf("[CASHIER %d]\t Got order.\n", cashier_id);

		/* Print sttaus that now the burger will be get from the rack */
		printf("[CASHIER %d]\t Going to rack to get burger...\n", cashier_id);

		/* Go to rack */
		sleep(rand() % WAITING_TIME);

		/* Aquire cashier semaphore */
		sem_wait(&cashier);

		/* Lock rack and get burger */
		sem_wait(&rack);
		assure_state();
		burger_count--;
		assure_state();
		sem_post(&rack);

		/* Signal a waiting cook a new burger can be produced */
		sem_post(&cook);

		/* Got successfull a burger. Print status */
		printf("[CASHIER %d]\t Got burger from rack, going back\n", cashier_id);

		/* Go back to customer */
		sleep(rand() % WAITING_TIME);

		/* Give burger to customer and print status */
		sem_post(&burger);
		printf("[CASHIER %d]\t Gave burger to customer.\n", cashier_id);
	}

	/* free semaphores and print message */
	sem_destroy(&order);
	sem_destroy(&burger);
	printf("[CASHIER %d]\t DONE.\n", cashier_id);

	return NULL;
}

void *customer_run(void *args) {
	/* Get args from void pointer */
	simple_arg_t *args_ptr = (simple_arg_t*) args;

	/* Get id */
	uint8_t customer_id = args_ptr->id;

	/* Print status and signal the init_done semaphore */
	printf("[CUSTOMER %d]\t CREATED.\n", customer_id);
	sem_post(args_ptr->init_done);

	/* Wait random time to mix customers (wait atleast 1s to assure that all
	   customers are already created) */
	sleep(rand() % WAITING_TIME + 1);

	/* Synchronize all customers to get a cashier. This will queue up the 
	   customers and guarantees every customer to get a cashier without race
	   conditions */
	sem_wait(&customer_private_mutex);

	/* Signal cashier that a customer is in the room and wait for a cashier to
	   wake up */
	sem_post(&customer);
	sem_wait(&cashier_awake);

	/* The cashier has placed his information in the cashier_exchange variable.
	   -> Get it! */
	sem_t *order = cashier_exchange.order;
	sem_t *burger = cashier_exchange.burger;
	uint8_t cashier_id = cashier_exchange.id;

	/* Leave syncronized area, now the next customer may aquire a cashier */
	sem_post(&customer_private_mutex);

	/* Print a status about the approached cashier */
	printf("[CUSTOMER %d]\t Approached cashier no. %d.\n", 
		customer_id, cashier_id);

	/* Print status that now the order will be placed */
	printf("[CUSTOMER %d]\t Placing order to cashier no. %d.\n", 
		customer_id, cashier_id);

	/* Place order */
	sleep(rand() % WAITING_TIME);

	/* Tell cashier about the order */
	sem_post(order);

	/* Wait for cashier to hand over the burger */
	sem_wait(burger);

	/* Process done, burger received. Print status and exit */
	printf("[CUSTOMER %d]\t Got burger from cashier no. %d. Thank you!\n", 
		customer_id, cashier_id);
	printf("[CUSTOMER %d]\t DONE.\n", customer_id);

	return NULL;

}

void assure_state() {
	/* Assure that more than zero burgers are available */
	if(burger_count < 0) {
		printf("[ASSURE_STATE]\t ERROR: Negative burger count!\n");
		exit(40);
	}

	/* Assure that not more burgers than rack spaces are available */
	if(burger_count > RACK_HOLDER_SIZE) {
		printf("[ASSURE_STATE]\t ERROR: Rack overfull!\n");
		exit(41);
	}

	printf("[ASSURE_STATE]\t State consistent.\n");
}