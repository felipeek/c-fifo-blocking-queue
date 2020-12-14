#define C_FEK_BLOCKING_QUEUE_IMPLEMENTATION
#define C_FEK_FAIR_LOCK_IMPLEMENTATION
#include "../blocking_queue.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define BLOCKING_QUEUE_CAPACITY 2

static Blocking_Queue bq;

static int num_threads;
static int rounds;

static int* produced;
static int* consumed;

static int current_produced_position;
static int current_consumed_position;

static pthread_t* producer_threads;
static pthread_t* consumer_threads;

void* producer(void* args) {
	assert(!blocking_queue_put(&bq, &produced[current_produced_position++]));
	return 0;
}

void* consumer(void* args) {
	void* got;
	blocking_queue_take(&bq, &got);
	assert(got != NULL);
	consumed[current_consumed_position++] = *(int*)got;
	return 0;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("usage: %s <num_threads> <rounds>\n", argv[0]);
		return -1;
	}

	num_threads = atoi(argv[1]);
	rounds = atoi(argv[2]);

	produced = malloc(num_threads * sizeof(int));
	consumed = malloc(num_threads * sizeof(int));
	producer_threads = malloc(num_threads * sizeof(pthread_t));
	consumer_threads = malloc(num_threads * sizeof(pthread_t));

	blocking_queue_init(&bq, BLOCKING_QUEUE_CAPACITY);

	for (unsigned int i = 0; i < num_threads; ++i) {
		produced[i] = i;
	}
	
	// This test is currently relying on sleeping to guess how much time it takes
	// for each thread to interact with the queue
	// This is obviously error-prone and should be better designed
	// This test may fail in some scenarios.

	for (unsigned int r = 0; r < rounds; ++r) {
		current_consumed_position = 0;
		current_produced_position = 0;

		for (unsigned int i = 0; i < num_threads; ++i) {
			if (pthread_create(&producer_threads[i], NULL, producer, NULL)) {
				fprintf(stderr, "error creating thread: %s\n", strerror(errno));
				return -1;
			}

			// This is error prone!
			usleep(5000);
		}

		for (unsigned int i = 0; i < num_threads; ++i) {
			if (pthread_create(&consumer_threads[i], NULL, consumer, NULL)) {
				fprintf(stderr, "error creating thread: %s\n", strerror(errno));
				return -1;
			}

			// This is error prone!
			usleep(5000);
		}

		for (unsigned int i = 0; i < num_threads; ++i) {
			pthread_join(producer_threads[i], NULL);
		}

		for (unsigned int i = 0; i < num_threads; ++i) {
			pthread_join(consumer_threads[i], NULL);
		}

		for (unsigned int i = 0; i < num_threads; ++i) {
			assert(produced[i] == consumed[i]);
		}
	}

	blocking_queue_destroy(&bq);
	free(produced);
	free(consumed);
	free(producer_threads);
	free(consumer_threads);

	printf("Test completed succesfully. [%u, %u]\n", num_threads, rounds);
	return 0;
}