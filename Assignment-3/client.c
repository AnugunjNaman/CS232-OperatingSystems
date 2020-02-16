/*
   @Anugunj Naman
   @1801022

   To run this file create object file using lpthread
   then $>./client
*/

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <mirror.h>
#include <pwd.h>
#include <sys/stat.h>
#include <grp.h>
#include <stdlib.h>
#include <pthread.h>

struct message_struct {
	long    mtype;         
	char command[1000];
	char msg[6000];
	int pid;
	int terminal;
}

void *input_func(int qid)
{
	cpl = 0;
	mymsgbuf msg;
	char command[100];
	strcpy(history_name, getenv("HOME"));
	strcpy(offset_name, history_name);
	char str[100];
	sprintf(str, "%d", self_pid);
	strcat(history_name, "/.myrp_history_");
	strcat(offset_name, "/.myrp_offset_");
	strcat(history_name, str);
	strcat(offset_name, str);
	int ext = 0;
	int flag = 0;
	while (1)
	{
		if (flag == 1)
		{
			flag = 0;
		}
		else
		{
			print_prompt();
		}

		fgets(command, 100, stdin);
		int is_bg = 0;
		int count = 0;
		char **args = parse_command(command, &count, &is_bg);
		if (count == 0)
			continue;
		char *command1 = args[0];
		if (strcmp(command1, "couple") == 0)
		{
			if (cpl != 0)
			{
				printf("Already Coupled\n");
				continue;
			}
			msg.pid = self_pid;
			msg.mtype = 1;
			//printf("Yes\n");
			if (send_message(qid, &msg) == -1)
			{
				perror("Error while sending message");
				to_exit(qid, cpl, command);


			}
			cpl = 1;
			flag = 1;
			continue;
		}
		//printf("%s\n",command1);
		if (strcmp(command1, "cd") == 0)
		{
			if (count < 2)
				sh_cd(NULL);
			else
				sh_cd(args[1]);
			if (cpl == 1)
			{
				strcpy(msg.command, command);
				msg.msg[0] = '\0';
				msg.mtype = 3;
				msg.terminal = tml_id;
				if (send_message(qid, &msg) == -1)
				{
					perror("Error while sending message");
					to_exit(qid, cpl, command);
				}
			}
			continue;
		}
		if (strcmp(command1, "clear") == 0)
		{
			sh_clear();
			if (cpl == 1)
			{
				strcpy(msg.command, command);
				msg.msg[0] = '\0';
				msg.mtype = 3;
				msg.terminal = tml_id;
				if (send_message(qid, &msg) == -1)
				{
					perror("Error while sending message");
					to_exit(qid, cpl, command);
				}
			}
			continue;
		}
		//printf("Yes\n");
		if (strcmp(command1, "exit") == 0)
		{
			int self_pid;
			msg.pid = self_pid;
			msg.mtype = 3;
			msg.terminal = tml_id;
			if (cpl == 1)
			{
				strcpy(msg.command, command);
				msg.msg[0] = '\0';
				msg.mtype = 2;
				msg.terminal = tml_id;
				if (send_message(qid, &msg) == -1)
				{
					perror("Error while sending message");
					to_exit(qid, cpl, command);
				}
			}
			cpl = 0;
			exit(0);
		}
		if (strcmp(command1, "history") == 0)
		{
			if (count < 2)
			{
				read_history(msg.msg);
			}
			else
			{
				int num = atoi(args[1]);
				read_history_arg(num, msg.msg);
			}
			if (cpl == 1)
			{
				strcpy(msg.command, command);
				msg.mtype = 3;
				msg.terminal = tml_id;
				if (send_message(qid, &msg) == -1)
				{
					perror("Error while sending message");
					to_exit(qid, cpl, command);
				}
			}
			continue;
		}
		if (strcmp(command1, "uncouple") == 0)
		{
			if(cpl==0)
			{
				printf("Already Uncoupled\n");
			}
			else
			{
				int self_pid;
				msg.pid = self_pid;
				msg.mtype = 2;
				msg.terminal = tml_id;
				msg.msg[0]='\0';
				strcpy(msg.command, command);
				if (send_message(qid, &msg) == -1)
				{
					perror("Error while sending message");
					to_exit(qid, cpl, command);
				}
				cpl = 0;
			}
			continue;
		}
		//printf("Yes\n");
		int pipes[2];
		pipe(pipes);
		if (fork() == 0)
		{
			dup2(pipes[1], STDOUT_FILENO);
			dup2(pipes[1], STDERR_FILENO);
			close(pipes[1]);
			close(pipes[0]);
			system(command);
			//printf("Hello\n");
			exit(0);
		}
		//printf("%d\n",is_bg);
		int status;
		wait(&status);
		close(pipes[1]);
		//printf("No\n");

		if (is_bg == 0)
		{
			char buf[BUF_SIZE] = {'\0'};
			int numRead = read(pipes[0], buf, BUF_SIZE);
			close(pipes[0]);
			//printf("%d\n",numRead);
			//printf("No\n");
			if (numRead == -1)
				perror("read");
			if (numRead == 0)
			{
				msg.msg[0] = '\0';
			}
			else
			{
				strcpy(msg.msg, buf);
			}
			printf("%s", buf);
			strcpy(msg.command, command);
			msg.terminal = tml_id;
			msg.pid = self_pid;
			msg.mtype = 3;
			if (cpl == 1)
			{
				if (send_message(qid, &msg) == -1)
				{
					perror("Error while sending message");
					to_exit(qid, cpl, command);
				}
			}
		}
		else
		{
			if (fork() == 0)
			{
				strcpy(msg.command, command);
				msg.msg[0] = '\0';

				msg.terminal = tml_id;
				msg.pid = getppid();
				msg.mtype = 3;
				if (cpl == 1)
				{
					if (send_message(qid, &msg) == -1)
					{
						perror("Error while sending message");
						to_exit(qid, cpl, command);
					}
				}
				char buf[BUF_SIZE] = {'\0'};
				int numRead = read(pipes[0], buf, BUF_SIZE);
				close(pipes[0]);
				if (numRead == -1)
					perror("read");
				if (numRead == 0)
				{
					msg.msg[0] = '\0';
				}
				else
				{
					strcpy(msg.msg, buf);
				}
				printf("%s", buf);
				if (numRead > 0)
				{   msg.command[0] = '\0';

					msg.terminal = tml_id;
					msg.pid = getppid();
					msg.mtype = 3;
					if (cpl == 1)
					{
						if (send_message(qid, &msg) == -1)
						{
							perror("Error while sending message");
							to_exit(qid, cpl, command);
						}
					}
				}
				exit(0);
			}
		}
	}
	pthread_exit(NULL);
}

void *output_func(int qid)
{
	mymsgbuf msg;
	while (1)
	{
		while (read_message(qid, self_pid, &msg) == -1)
		{
			perror("Error while reading message");
			char str[10] = "";
			to_exit(qid, cpl, str);
		}
		if (msg.pid == -1)
		{
			tml_id = msg.terminal;
			printf("ID :%d \n", msg.terminal);
		}
		else {
			printf("\nTerminal %d: ", msg.terminal);
			printf("%s", msg.command);
			printf("%s", msg.msg);
		}
		print_prompt();
		fflush(stdout);
	}
	pthread_exit(NULL);
}


int main(){
	pthread_t threads[2];
	self_pid = getpid();
	int rc;
	key_t key = 1234;
	int queue_id = msgget(key, IPC_CREAT | 0666);
	printf("Queue_id: %d\n", queue_id);
	long i;
	i = 0;
	rc = pthread_create(&threads[i], NULL,
			input_func, queue_id);
	i = 1;
	if (rc) {
		printf("Error:unable to create thread, %d\n", rc);
		exit(-1);
	}
	rc = pthread_create(&threads[i], NULL,
			output_func, queue_id);
	if (rc) {
		printf("Error:unable to create thread, %d\n", rc);
		exit(-1);
	}
	pthread_exit(NULL);
	return 0;
}
