//OS-P3 2022-2023

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stddef.h>
#include <sys/stat.h>
#include <pthread.h>
#include "queue.h"
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>


/**
 * Entry point
 * @param argc
 * @param argv
* @return
 */

//global variables and constants:
#define MAX_OPERATIONS 200

int client_numop = 0; //counter of productions
int bank_numop = 0; //counter of consumptions
int global_balance = 0; //total balance of the bank
int end = 0;
struct element *list_client_ops; //dynamic array of operations 
queue *CircularQueue;  //circular queue (buffer) shared by producers and consumer threads
int *balance; //dynamic array of integers (of size = <max_account>) that stores the ith account current balance at ith index
pthread_mutex_t mutex; //mutex lock to synchronize threads
pthread_cond_t full; //condition variable on the queue being full 
pthread_cond_t empty; //condition variable on the queue being empty

void *Atm(void *params) //producer thread function
{
	int amount_to_produce = *((int*)params); //typecast the void pointer params and dereference it in order to obtain the actual number of operations to process (the one in the first line of the file)
	
	while(client_numop < amount_to_produce) //producer(atm) must work while there are operations to produce
	{
		//enter critical section
		pthread_mutex_lock(&mutex);
		/*
		printf("A producer has won the lock\n");
		printf("Client_numop -> %d\n",client_numop);
		printf("queue_full(&CircularQueue) -> %d\n", queue_full(CircularQueue));
		*/
		
		while(queue_full(CircularQueue)) //wait for the queue to be dequeued ie : not full
		{
			pthread_cond_wait(&full, &mutex); //waits on signal of full (made by consumers after consuming)
		}
		queue_put(CircularQueue, &(list_client_ops[client_numop])); //enqueue the operation from the array at index client_numop
		//printf("Produced\n");
		client_numop = client_numop +1; //increment by 1 the number of productions done
		pthread_cond_signal(&empty); //signal that the queue is not empty anymore 
		pthread_mutex_unlock(&mutex); //unlock the mutex
	}

	//terminate execution
	pthread_exit(0);
	return NULL;
}


void *Worker(void *params) //consumer thread function
{
	int amount_to_consume = *((int*)params); //typecast the void pointer params as an int pointer and dereference it to access the number of ops to process (the one in the first line of the input file)
	
	while(bank_numop < amount_to_consume) //each worker must consume as long as there are operations left to consume
	{
		//enter critical section
		pthread_mutex_lock(&mutex);
		/*
		printf("A worker has won the lock\n");
		printf("Bank_numop -> %d\n",bank_numop);
		printf("queue_empty(CircularQueue) -> %d\n", queue_empty(CircularQueue));
		*/
		
		while(queue_empty(CircularQueue)) //wait for the queue to be enqued ie: not empty
		{
			if(end == 1) //empty and finished
			{
				pthread_mutex_unlock(&mutex);
				pthread_exit(0);
			}
			pthread_cond_wait(&empty,&mutex); //waits on signal of empty (made by producers after producing)
		}
		//printf("Consumed!\n");
		struct element *data = queue_get(CircularQueue); //dequeue the operation on the circular queue
		take_action(data, balance, global_balance, bank_numop); //take the respective action and print info on screen
		bank_numop = bank_numop +1; //increment by 1 the number of consumptions done
		pthread_cond_signal(&full); //signal to a producer that the queue is not full
		pthread_mutex_unlock(&mutex); //unlock the mutex
	}
	//terminate execution
	pthread_exit(0);
	return NULL;
}

int main (int argc, const char * argv[] ) {
    /* NOTES:
    -The program is executed on the following way -> ./bank <file_name> <num_ATMS's> <num_workers> <max_accounts> <queue_size>
    */ 
   //store the input data inside variables for readability
    char *file_name = argv[1];
    int num_atms = atoi(argv[2]);
    int num_workers = atoi(argv[3]);
    int max_accounts = atoi(argv[4]);
    int qsize = atoi(argv[5]);
	printf("File name is : %s, Number of atms -> %d, Number of workers -> %d, Max accounts is -> %d, Queue size -> %d\n",file_name,num_atms,num_workers,max_accounts,qsize); 

//1- Main process reads input file and stores operations inside the array of type elements/////////////////////
    
	list_client_ops = (struct elememt*)malloc(MAX_OPERATIONS*sizeof(struct element));
	balance = (int*)malloc(max_accounts*sizeof(int)); //array of balances of size the maximum number of accounts given as input
	int num_ops = read_operations(file_name, list_client_ops);
	//printf("Number of operations inside file -> %d\n",num_ops);
	if(num_ops == -1)
	{
		printf("Error when reading from the input file!\n");
		return -1;
	}
	/*
	for(int i = 0; i<num_ops; i++)
    {
        printf("%d-operation id -> %d , account number -> %d , account from -> %d, account to -> %d , amount -> %f \n",i+1, list_client_ops[i].operation_id, list_client_ops[i].account_number, list_client_ops[i].acc_from, list_client_ops[i].acc_to, list_client_ops[i].amount);
    }
	*/
   
   //creation of producer and consumer threads based on input
   pthread_t Tids[num_atms+num_workers];
   CircularQueue = queue_init(qsize); //initializes the circular queue of size <queue size> for producer and consumer threads (visible to all threads)
   pthread_mutex_init(&mutex, NULL); //initializes mutex lock (visible to all threads)
   pthread_cond_init(&full, NULL);
   pthread_cond_init(&empty, NULL);
   printf("Number of operations > %d\n",num_ops);
   for(int i = 0; i<num_atms; i++)
   {
	 if(pthread_create(&(Tids[i]), NULL, Atm, &num_ops) !=0) //each producer is given as parameter the number of operations to produce
	 {
		printf("Error creating the producer threads\n");
		return -1;
	 }

   }

   for(int j = 0; j<num_workers; j++)
   {
	 if(pthread_create(&(Tids[j+num_atms]), NULL, Worker, &num_ops) !=0) //each worker is given as parameter the number of operations to consume
	 {
		printf("Error creating the producer threads\n");
		return -1;
	 }
	 //wait between each thread creation since these receive non null parameter
   }

   for(int i = 0; i<num_atms; i++) //wait for producers to finish
   {
	 pthread_join(Tids[i],NULL);
   }
   
   //end of execution of all the consumer threads:
   pthread_mutex_lock(&mutex);
   end = 1;
   pthread_cond_broadcast(&empty);
   pthread_mutex_unlock(&mutex);


   for(int j = 0; j<num_workers; j++) //wait for consumers to finish
   {
	 pthread_join(Tids[j+num_atms],NULL); 
   }


    pthread_mutex_destroy(&mutex); //destroy the mutex
	pthread_cond_destroy(&full); //destroy all condition variables
	pthread_cond_destroy(&empty);
	queue_destroy(CircularQueue);//destroy the circular queue
	free(list_client_ops); //free memory used by array of elements
    return 0;
}
