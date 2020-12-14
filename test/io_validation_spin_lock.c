#define C_FEK_BLOCKING_QUEUE_IMPLEMENTATION
#define C_FEK_FAIR_LOCK_IMPLEMENTATION
#include "../blocking_queue.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

static Blocking_Queue bq;

static int data_size;
static int num_producer_threads;
static int num_consumer_threads;

static int* produced;
static int* consumed;

static int* producer_threads_ids;
static int* consumer_threads_ids;
static pthread_t* producer_threads;
static pthread_t* consumer_threads;

#define BLOCKING_QUEUE_CAPACITY 2

static void heapsort(int a[], int n) {
	int i = n / 2, parent, child, t;
	while (1) {
		if (i > 0) {
			i--;
			t = a[i];
		} else {
			n--;
			if (n <= 0) return;
			t = a[n];
			a[n] = a[0];
		}
		parent = i;
		child = i * 2 + 1;
		while (child < n) {
			if ((child + 1 < n) && (a[child + 1] > a[child]))
				child++;
			if (a[child] > t) {
				a[parent] = a[child];
				parent = child;
				child = parent * 2 + 1;
			} else {
				break;
			}
		}
		a[parent] = t;
	}
}

void blocking_queue_add_spin_lock(Blocking_Queue* bq, void* data) {
	int ret;
	do {
		ret = blocking_queue_add(bq, data);
		assert(ret == 0 || ret == BQ_FULL);
	} while (ret != 0);
}

void* blocking_queue_get_spin_lock(Blocking_Queue* bq) {
	void* out;
	int ret;
	do {
		ret = blocking_queue_poll(bq, &out);
		assert(ret == 0 || ret == BQ_EMPTY);
	} while (ret != 0);
	return out;
}

void* producer(void* args) {
	int producer_id = *(int*)args;
	unsigned int num_data_to_consume = data_size / num_producer_threads;
	unsigned int start_at = producer_id * num_data_to_consume;

	for (unsigned int i = start_at; i < start_at + num_data_to_consume; ++i) {
		blocking_queue_add_spin_lock(&bq, &produced[i]);
	}

	return 0;
}

void* consumer(void* args) {
	int consumer_id = *(int*)args;
	unsigned int num_data_to_consume = data_size / num_consumer_threads;
	unsigned int start_at = consumer_id * num_data_to_consume;

	for (unsigned int i = start_at; i < start_at + num_data_to_consume; ++i) {
		void* got = blocking_queue_get_spin_lock(&bq);
		consumed[i] = *(int*)got;
	}

	return 0;
}

int main(int argc, char** argv) {
	if (argc != 4) {
		printf("usage: %s <num_producer_threads> <num_consumer_threads> <data_size>\n", argv[0]);
		return -1;
	}

	num_producer_threads = atoi(argv[1]);
	num_consumer_threads = atoi(argv[2]);
	data_size = atoi(argv[3]);
	assert(data_size % num_producer_threads == 0);
	assert(data_size % num_consumer_threads == 0);

	produced = malloc(data_size * sizeof(int));
	consumed = malloc(data_size * sizeof(int));
	producer_threads_ids = malloc(num_producer_threads * sizeof(int));
	consumer_threads_ids = malloc(num_consumer_threads * sizeof(int));
	producer_threads = malloc(num_producer_threads * sizeof(pthread_t));
	consumer_threads = malloc(num_consumer_threads * sizeof(pthread_t));

	blocking_queue_init(&bq, BLOCKING_QUEUE_CAPACITY);

	for (unsigned int i = 0; i < data_size; ++i) {
		produced[i] = i;
	}

	for (unsigned int i = 0; i < num_producer_threads; ++i) {
		producer_threads_ids[i] = i;
		if (pthread_create(&producer_threads[i], NULL, producer, &producer_threads_ids[i])) {
			fprintf(stderr, "error creating thread: %s\n", strerror(errno));
			return -1;
		}
	}

	for (unsigned int i = 0; i < num_consumer_threads; ++i) {
		consumer_threads_ids[i] = i;
		if (pthread_create(&consumer_threads[i], NULL, consumer, &consumer_threads_ids[i])) {
			fprintf(stderr, "error creating thread: %s\n", strerror(errno));
			return -1;
		}
	}

	for (unsigned int i = 0; i < num_producer_threads; ++i) {
		pthread_join(producer_threads[i], NULL);
	}

	for (unsigned int i = 0; i < num_consumer_threads; ++i) {
		pthread_join(consumer_threads[i], NULL);
	}

	heapsort(consumed, data_size);

	for (unsigned int i = 0; i < data_size; ++i) {
		assert(produced[i] == consumed[i]);
	}

	blocking_queue_destroy(&bq);
	free(produced);
	free(consumed);
	free(producer_threads_ids);
	free(consumer_threads_ids);
	free(producer_threads);
	free(consumer_threads);

	printf("Test completed succesfully. [%u, %u, %u]\n", num_producer_threads, num_consumer_threads, data_size);
	return 0;
}