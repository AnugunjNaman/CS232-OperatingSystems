/*
   @Anugunj Naman
   @1801022

   To run this file create object file using lpthread
   then $>./server
  */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <mirror.h>
#include <sys/msg.h>
#include <signal.h>
#include <pthread.h>
#define _GNU_SOURCE

int send_message( int qid, struct mymsgbuf *qbuf ){
	int     result, length;
	length = sizeof(struct mymsgbuf) - sizeof(long);       
	if((result = msgsnd( qid, qbuf, length, 0)) == -1){
		return(-1);
	}
	return(result);
}
int read_message( int qid, long type, struct mymsgbuf *qbuf ){
	int     result, length;
	length = sizeof(struct mymsgbuf) - sizeof(long);        
	if((result = msgrcv( qid, qbuf, length, type,  0)) == -1){
		return(-1);
	}
	return(result);
}

int main() {
	key_t key=1234;
	qid=msgget(key,IPC_CREAT|0666);
	if(change_queue_size(qid)==-1)
	{
		perror("Error while changing the size of Message queue");
		exit(1);
	}
	int j=0;
	for(j=0;j<10000;j++)
		poid[j]=-1;
	printf("qid : %d\n",qid);
	pthread_t threads[3];
	int rc;
	long i;
	i=0;
	rc = pthread_create(&threads[i], NULL, 
			cpl, qid);
	i=1;
	if (rc){
		printf("Error:unable to create thread, %d\n",rc);
		return 0;
	}
	rc = pthread_create(&threads[i], NULL, 
			uncpl, qid);
	if (rc){
		printf("Error:unable to create thread, %d\n",rc);
		return 0;
	}
	i=2;
	rc = pthread_create(&threads[i], NULL, 
			rcvsend, qid);
	if (rc){
		printf("Error:unable to create thread, %d\n",rc);
		return 0;
	} 
	signal(SIGINT,ctrlc);
	pthread_exit(NULL);
	return 0;
}

void ctrlc()
{
	msgctl(qid,IPC_RMID,NULL);
	exit(0);
}


