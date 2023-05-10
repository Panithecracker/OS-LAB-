#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
	// Define the struct yourself
	int param1;
};

typedef struct queue {
	int head; //index for the first element in the queue
    int tail; //index for the last element in the queue
    int size; // fixed amount of data capable of storing
    int *data; // actual array of length size of data to store
}queue;

queue* queue_init (int size);
int queue_empty (queue *q);
int queue_full(queue *q);
int queue_put (queue *q, struct element* elem);
struct element * queue_get(queue *q);
int queue_destroy (queue *q);

#endif

