#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

struct sum_runner_struct{
	long long limit;
	long long answer;
};



void* sum_runner(void* arg){
	struct sum_runner_struct *arg_struct = (struct sum_runner_struct*)arg;
	long long sum = 0;
	for(long long i = 0; i <= arg_struct->limit; i++){
		sum += i;
	}
	arg_struct->answer = sum;
	pthread_exit(0);

}

int main(int argc, char** argv){
	if(argc < 2){
		printf("Usage: %s <num1> <num2> ...... <num-n>\n", argv[1]);
		exit(-1);
	}
	
	int num_args = argc - 1;
	pthread_t tid[num_args];
	struct sum_runner_struct args[num_args];
	int i = 0, j = 0;
	for(i = 0; i < num_args; i++){

		args[i].limit = atoll(argv[i+1]);
		pthread_attr_t attr;
	   pthread_attr_init(&attr);
		pthread_create(&tid[i], &attr, sum_runner, &args[i]);
	}
	
	for(j = 0; j < num_args; j++){
		pthread_join(tid[j], NULL);
		printf("Sum of thread %d  is %lld\n", j, args[j].answer);
	}


	return 0;

}
