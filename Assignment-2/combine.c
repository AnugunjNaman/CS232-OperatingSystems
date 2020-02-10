/*
 * @author: Anugunj Naman
 * @for : OSLAB 3
 */

#include<signal.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>


/* Not all headers file on native Windows System. Not supported in MinGW 32/64. Not checked on Cygwin*/

int main(int argc, char *argv[]){
        int status;
        /*Checking if two files are provided as input*/
        if(argc != 3){
                fprintf(stderr, "Dobe! You need to provide two arguments\n");
                exit(1);
        }
        /*Read from file1*/
        int fd_in = open(argv[1], O_RDONLY);
        if(fd_in == -1) {
                fprintf(stderr, "There was a problem reading %s. Baka\n", argv[1]);
                exit(1);
        }
        /*Read from file2*/
        int fd_out = creat(argv[2], 0644);
        if(fd_out == -1)        {
                fprintf(stderr, "There was a problem creating %s. Baka\n", argv[2]);
                exit(1);
        }

        /*Closing input file descripter for file1 to be input*/
        if(close(0) == -1){
                fprintf(stderr, "Error Dobe!!\n");
                exit(1);
        }
        if(dup(fd_in)==-1){
                fprintf(stderr, "Error in dup(). Dobe!!\n");
                exit(1);
        }

        /*Closing output file descripter for file2 to be output*/
        if(close(1) == -1){
                fprintf(stderr, "Error Dobe!!\n");
                exit(1);
        }
        if(dup(fd_out)== -1){
                fprintf(stderr, "Error in dup(). Dobe!!\n");
                exit(1);
        }

        if(close(fd_in) == -1){
                fprintf(stderr, "Error Dobe!!\n");
                exit(1);
        }
        if(close(fd_out) == -1) {
                fprintf(stderr, "Error Dobe!!\n");
                exit(1);
	}
	/*Creating file descriptor array*/ 
       int fd[2];
        pipe(fd);
	/*Forking two process*/
        if(fork()!=0){
                if(fork()!=0){
                        if(close(fd[0]) == -1){
                                fprintf(stderr, "Error Dobe!!\n");
                                exit(1);
                        }
                        if(close(fd[1]) == -1){
                                fprintf(stderr, "Error Dobe!!\n");
                                exit(1);
                        }
                        wait(&status);
                        wait(&status);
			/*Waiting for child process*/
                }
                else{
                        if(close(1) == -1){
                                fprintf(stderr, "Error Dobe!!\n");
                                exit(1);
                        }
                        dup(fd[1]);
                        if(close(fd[0]) == -1)  {
                                fprintf(stderr, "Error Dobe!!\n");
                                exit(1);
                        }
                        if(close(fd[1]) == -1){
                                fprintf(stderr, "Error Dobe!!\n");
                                exit(1);
                        }
                        execl("switch", "switch", (char*)NULL);
                }
        }
        else{ 
                if(close(0) == -1){
                        fprintf(stderr, "Error Dobe!!\n");
                        exit(1);
                }
                dup(fd[0]);
                if(close(fd[0]) == -1){
                        fprintf(stderr, "Error Dobe!!\n");
                        exit(1);
                }
                if(close(fd[1]) == -1){
                        fprintf(stderr, "Error Dobe!!\n");
                        exit(1);
                }
                execl("count", "count", (char*)NULL);

        }
        return 0;
}

