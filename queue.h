#ifndef HEADER_FILE
#define HEADER_FILE


struct element {
    int account_number;
    int acc_from;
    int acc_to;
    float amount;
    int operation_id;
	
};

typedef struct queue {
	int head; //index for the first element in the queue
    int tail; //index for the last element in the queue
    int size; // fixed amount of data capable of storing
    struct element *data; // actual array of length size of element type 
}queue;

queue* queue_init (int size);
int queue_empty (queue *cq);
int queue_full(queue *cq);
int queue_put (queue *cq, struct element* e);
struct element * queue_get(queue *cq);
int queue_destroy (queue *cq);

//function that does the reading and storing info inside array of elements 
int read_operations(char *file, struct element *list_client_ops);

//function that performs an ATM request given an operation ie : element object, which potentially accesses balance array and global balance variable
int take_action(struct element *data, int *balance, int global_balance, int bank_numop);


#endif
