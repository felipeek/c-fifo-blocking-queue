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
#define TIME_TO_DESTROY_MS 50

static Blocking_Queue bq;

static int num_producer_threads;
static int num_consumer_threads;

static pthread_t* producer_threads;
static pthread_t* consumer_threads;

void* producer(void* args) {
	while (1) {
		int ret = blocking_queue_put(&bq, 0);
		assert(ret == 0 || ret == BQ_CLOSED);
		if (ret == BQ_CLOSED) {
			break;
		}
	}

	return 0;
}

void* consumer(void* args) {
	while (1) {
		void* got;
		int ret = blocking_queue_take(&bq, &got);
		assert(ret == 0 || ret == BQ_CLOSED);
		if (ret == BQ_CLOSED) {
			break;
		}
	}

	return 0;
}

int main(int argc, char** argv) {
	if (argc != 3) {
		printf("usage: %s <num_producer_threads> <num_consumer_threads>\n", argv[0]);
		return -1;
	}

	num_producer_threads = atoi(argv[1]);
	num_consumer_threads = atoi(argv[2]);

	producer_threads = malloc(num_producer_threads * sizeof(pthread_t));
	consumer_threads = malloc(num_consumer_threads * sizeof(pthread_t));

	blocking_queue_init(&bq, BLOCKING_QUEUE_CAPACITY);

	for (unsigned int i = 0; i < num_producer_threads; ++i) {
		if (pthread_create(&producer_threads[i], NULL, producer, NULL)) {
			fprintf(stderr, "error creating thread: %s\n", strerror(errno));
			return -1;
		}
	}

	for (unsigned int i = 0; i < num_consumer_threads; ++i) {
		if (pthread_create(&consumer_threads[i], NULL, consumer, NULL)) {
			fprintf(stderr, "error creating thread: %s\n", strerror(errno));
			return -1;
		}
	}

	usleep(TIME_TO_DESTROY_MS * 1000);
	blocking_queue_close(&bq);

	for (unsigned int i = 0; i < num_producer_threads; ++i) {
		pthread_join(producer_threads[i], NULL);
	}

	for (unsigned int i = 0; i < num_consumer_threads; ++i) {
		pthread_join(consumer_threads[i], NULL);
	}

	blocking_queue_destroy(&bq);
	free(producer_threads);
	free(consumer_threads);

	printf("Test completed succesfully. [%u, %u]\n", num_producer_threads, num_consumer_threads);
	return 0;
}