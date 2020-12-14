# C FIFO Blocking Queue

To use this blocking queue, define `C_FEK_BLOCKING_QUEUE_IMPLEMENTATION` before including blocking_queue.h in one of your source files.

To use this blocking queue, you must link your binary with pthread.

Note that fair_lock.h is a pre-requisite for this implementation, so you also need to include fair_lock.h in one of your source files
and define `C_FEK_FAIR_LOCK_IMPLEMENTATION` before including it.

This blocking queue is thread-safe.

This blocking queue has the following properties/features:

- It has a fixed capacity, provided by the caller.
- It allows the caller to add elements to the queue via both a blocking call and a non-blocking call.
- It allows the caller to get elements from the queue via both a blocking call and a non-blocking call.
- If multiple callers are blocked adding/getting an element to/from the queue, they are served in FIFO order.

The last point avoids the problem of starvation.

Note that since the queue is designed to serve callers in FIFO order, this might have a significant impact in performance.

If FIFO is not needed in your implementation, consider using a queue with no such guarantee.

Define `C_FEK_BLOCKING_QUEUE_NO_CRT` if you don't want the C Runtime Library included. If this is defined, you must provide
implementations for the following functions:

```c
void* malloc(size_t size)
void free(void* block)
```

For more information about the API, check the comments in the function signatures.

A usage example:

```c
#define C_FEK_BLOCKING_QUEUE_IMPLEMENTATION
#define C_FEK_FAIR_LOCK_IMPLEMENTATION
#include "blocking_queue.h"
#include <assert.h>

int main() {
	Blocking_Queue bq;
	int aux;

	blocking_queue_init(&bq, 4);
	
	// Example of non-blocking calls
	blocking_queue_add(&bq, (void*)1);
	blocking_queue_poll(&bq, &aux);
	assert(aux == 1);

	// Example of potentially blocking calls
	blocking_queue_put(&bq, (void*)2);
	blocking_queue_take(&bq, &aux);
	assert(aux == 2);

	blocking_queue_destroy(&bq);
	return 0;
}
```

Another example - more similar to a real use-case - since this time the integer is allocated:

```c
#define C_FEK_BLOCKING_QUEUE_IMPLEMENTATION
#define C_FEK_FAIR_LOCK_IMPLEMENTATION
#include "blocking_queue.h"
#include <assert.h>

int main() {
	Blocking_Queue bq;
	int* aux = malloc(sizeof(int));
	int* aux2;

	blocking_queue_init(&bq, 4);
	
	// Example of non-blocking calls
	*aux = 1;
	blocking_queue_add(&bq, aux);
	blocking_queue_poll(&bq, &aux2);
	assert(*aux2 == 1);

	// Example of potentially blocking calls
	*aux = 2;
	blocking_queue_put(&bq, aux);
	blocking_queue_take(&bq, &aux2);
	assert(*aux2 == 2);

	blocking_queue_destroy(&bq);
	free(aux);
	return 0;
}
```