//OS-P3 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include "queue.h"



//To create a queue
queue* queue_init(int size){
	queue *cq = (queue*)malloc(sizeof(queue)); //allocate a cq in the heap
    cq->size = size;
    cq->data = (struct element*)malloc(size*sizeof(struct element));//allocate the array it holds as content attribute in the heap
    cq->head = -1; //by default, initially head and tail are both equal to -1
    cq->tail = -1; 
    return cq;
}

//To check queue state
int queue_empty(queue *cq){
	if(cq->head == -1 && cq->tail == -1) //by default this means that the queue is empty
    {
        return 1;
    }
    return 0;
}

int queue_full(queue *cq){
	if(queue_empty(cq))
    {
        return 0;
    }
    if((cq->tail +1)%cq->size == cq->head)
    {
        return 1;
    }
    return 0;
}

// To Enqueue an element
int queue_put(queue *cq, struct element *e) {
	if(!queue_full(cq)) //check first if the queue is full 
    {
        if(queue_empty(cq)) //then, in this special case, recall that we must set head to 0 since it is -1
        {
            cq->head = 0;
        }
        cq->data[(cq->tail+1)%cq->size] = *e; //then, insert it at tail+1 and update the value of tail
        cq->tail = (cq->tail +1)%cq->size;
        return 1;
    }
    return -1;
}


// To Dequeue an element.
struct element* queue_get(queue *cq) {
	if(!queue_empty(cq)) //check first if the queue is empty already
    {
        struct element *e = &cq->data[cq->head]; //element to drop off the queue
        if(cq->head == cq->tail) //only one element so after dequeing, set the head and tail to -1
        {
            cq->head = -1;
            cq->tail = -1;
            return e;
        }
        //if not the only one element then, increment modulo size the value of head
        cq->head = (cq->head+1)%cq->size;
        return e;
    }
    return NULL;
}

//To destroy the queue and free the resources
int queue_destroy(queue *cq){
	//first, free its content array from the heap and then the object pointer
    free(cq->data);
    free(cq);
	return 0;
}

//function that does the reading and storing info inside array of elements (it modifies the input element array + returns the first line integer on the file referring to the number of operations )
int read_operations(char *file, struct element *list_client_ops)
{
    FILE *fp = fopen(file, "r");
	    if (fp == NULL) {
		perror("Error opening file");
		return -1;
	    }
	    
	    char line[100];
		int numops = 0; 
		fgets(line, 100, fp);
		numops = atoi(line);
	    int i = 0;
	    while (fgets(line, 100, fp) != NULL) {
	    	// Initialize amount to 0 for each new element
        	list_client_ops[i].amount = 0.0;
	    
		char *token = strtok(line, " ");
		if (strcmp(token, "CREATE") == 0) {
		    token = strtok(NULL, " ");
		    list_client_ops[i].operation_id = 1;
		    list_client_ops[i].account_number = atoi(token);
		} else if (strcmp(token, "DEPOSIT") == 0) {
		    token = strtok(NULL, " ");
		    list_client_ops[i].operation_id = 2;
		    list_client_ops[i].account_number = atoi(token);
		    token = strtok(NULL, " ");
		    list_client_ops[i].amount = atof(token);
		} else if (strcmp(token, "TRANSFER") == 0) {
		    token = strtok(NULL, " ");
		    list_client_ops[i].operation_id = 3;
		    list_client_ops[i].acc_from = atoi(token);
		    token = strtok(NULL, " ");
		    list_client_ops[i].acc_to = atoi(token);
		    token = strtok(NULL, " ");
		    list_client_ops[i].amount = atof(token);
		} else if (strcmp(token, "WITHDRAW") == 0) {
		    token = strtok(NULL, " ");
		    list_client_ops[i].operation_id = 4;
		    list_client_ops[i].account_number = atoi(token);
		    token = strtok(NULL, " ");
		    list_client_ops[i].amount = atof(token);
		} else if (strcmp(token, "BALANCE") == 0) {
		    token = strtok(NULL, " ");
		    list_client_ops[i].operation_id = 5;
		    list_client_ops[i].account_number = atoi(token);
		}
		i++;
	    }
    
	    fclose(fp);
        return numops;
}


//function that performs an ATM request given an operation ie : element object, which potentially accesses balance array and global balance variable
int take_action(struct element *data, int *balance, int *global_balance, int bank_numop)
{
		switch (data->operation_id) {
		    case 1: // CREATE
		      balance[data->account_number] = 0;
		      printf("%d CREATE %d BALANCE=%d TOTAL=%d\n", bank_numop+1, data->account_number, balance[data->account_number], *global_balance);
		      return 0;
		    case 2: // DEPOSIT
		      balance[data->account_number] += data->amount;
		      *global_balance += data->amount;
		      printf("%d DEPOSIT %d %d BALANCE=%d TOTAL=%d\n", bank_numop+1, data->account_number, data->amount, balance[data->account_number], *global_balance);
			  return 0;
		    case 3: // TRANSFER
		      // Total stays the same (Balance of the accounts change)
		      balance[data->acc_from] -= data->amount;
		      balance[data->acc_to] += data->amount;
		      printf("%d TRANSFER %d %d %d BALANCE=%d TOTAL=%d\n", bank_numop+1, data->acc_from, data->acc_to, data->amount, balance[data->acc_to], *global_balance);
			  return 0;
		    case 4: // WITHDRAW
		      balance[data->account_number] -= data->amount;
		      *global_balance -= data->amount;
		      printf("%d WITHDRAW %d %d BALANCE=%d TOTAL=%d\n", bank_numop+1, data->account_number, data->amount, balance[data->account_number], *global_balance);
		   	  return 0;
		    case 5: // BALANCE
		      // Nada creo
		      printf("%d BALANCE %d BALANCE=%d TOTAL=%d\n", bank_numop+1, data->account_number, balance[data->account_number], *global_balance);
		      return 0;
		    default:
		      perror("Not valid operation");
		}
}
