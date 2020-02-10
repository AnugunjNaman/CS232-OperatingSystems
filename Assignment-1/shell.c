/*
 * @author: Anugunj Naman
 * @for : OSLAB 2
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
#include<sys/stat.h>

/* Not all headers file on native Windows System. Not supported in MinGW 32/64. Not checked on Cygwin*/

int size = 500; 
int input_size = 500; 
int num = 0;
int shell_pid;
int *arglen; 
char *pointer_to_home; 
char *** history; 
char **process;
int *pid;
int num_process;
char **process_current;
int *pid_current;
int num_current;
/* List of several variables which may/may not be used during this whole writing of file*/

void hist1(char **,int);
void hist(char **,int);
void pid_finder(char **,int);
void cd(char **,int);
void other(char **,int,int);
void custom_pipe_function(char **,int);
void redirection_pipe_function(char **,int);
void redirection_func(char **,int);
void function_identifier(char **,int);

/*Zombie State: On Unix and Unix-like computer operating systems, 
a zombie process or defunct process is a process that has completed execution
(via the exit system call) but still has an entry in the process table: it is a process in the "Terminated state".*/


/*This function handles the zombie state of child process. It is used as func when SIGCHLD signal is recieved it uses wait function 
so the parent collects the exit status almost immediately and the child entry in the process table is cleared*/

void bg_handler(int para)
{
    int i;
    int status;

    pid_t childpid = wait(&status); /*returns the process id of the terminated process*/
    if(childpid == -1)
        perror("Error Dobe!!"); 

    for(i=0; i<num_current; i++)
        if(pid_current[i] == childpid)
            break; 

    if(i != num_current)
    {
        if(WIFEXITED(status)) /*To know exit status of the child process*/
        {
            printf("Process %s with pid %d exited properly without error\n",process_current[i],pid_current[i]);
            printf("Press Enter to continue\n");
        }
        else
        {
            printf("Process %s with pid %d did not exit exited properly\n",process_current[i],pid_current[i]);
            printf("Press enter to continue\n");
        }
        pid_current[i] = 0;
        process_current[i] = NULL;
    }
}

/*Custom change directory function for this custom shell.*/

void cd(char **arg,int len)
{
    char tmp[size];
    int ret;

    if(len == 1)
        ret = chdir(pointer_to_home);
    else if(arg[1][0] == '~' && arg[1][1]=='\0')
        ret = chdir(pointer_to_home);
    else
    {
        if(arg[1][0] == '~')
        {
            strcpy(tmp,pointer_to_home);
            strcat(tmp,&arg[1][1]);
            ret = chdir(tmp);
        }
        else
            ret = chdir(arg[1]);
    }

    if(ret == -1)
        perror("Error Dobe!!");
    else
    {
        strcpy(process[num_process],arg[0]);
        pid[num_process++] = shell_pid;
    }
}

/*Custom history command for the custom shell. Simple hist list all previous commands for the current session
 while histXX gives all previous XX sessions*/ 

void hist(char **arg,int len){
    if(len != 1){
        fprintf(stderr,"Invalid Command Dobe!!\n");
        return;
    }

    int i = 0, j = 0, flag = 0;
    char temp[30];

    if(arg[0][4] == '\0'){
        for(i=0; i<num; i++){
            for(j=0; j<arglen[i]; j++){
                printf("%s ",history[i][j]);
            }
            printf("\n");
        }
    }
    else{
        for(i=4; i<strlen(arg[0]); i++){
            if(arg[0][i]<'0' || arg[0][i]>'9'){
                flag = 1;
                break;
            }
            temp[j++] = arg[0][i];
        }
        temp[j] = '\0';

        if(flag == 1){
            fprintf(stderr,"Invalid Command Dobe!!\n");
            return;
        }

        flag = atoi(temp);

        if(num < flag)
            flag = num;

        for(i=num-flag; i<num; i++){
            for(j=0; j<arglen[i]; j++){
                printf("%s ",history[i][j]);
            }
            printf("\n");
        }
    }

    strcpy(process[num_process],arg[0]);
    pid[num_process++] = shell_pid;
}

/*Function for finding pid for different types of pid, pid all gives all etc*/

void pid_finder(char **arg,int len){
    int i, flag = 0;

    if(len == 1){
        printf("Command name: ./a.out process id: %d\n",shell_pid);
        flag = 1;
    }
    else if(strcmp(arg[1],"all") == 0){   
        for(i=0; i<num_process; i++){   
            printf("Command name: %s process id: %d\n",process[i],pid[i]);
        }   
        flag = 1;
    }   
    else if(strcmp(arg[1],"current") == 0){ 
        for(i=0; i<num_current; i++){   
            if(pid_current[i] != 0){   
                printf("Command name: %s process id: %d\n",process_current[i],pid_current[i]);
            }   
        }   
        flag = 1;
    }   
    else
        fprintf(stderr,"Command not found\n");
    if(flag == 1){
        strcpy(process[num_process],arg[0]);
        pid[num_process++] = shell_pid;
    }
}

/*For other function we use this creating process and handling background processes*/

void other(char **arg,int len,int bg){
    int flag = 0;
    int childpid;

    pid_t ret = vfork();
    if(ret < 0)
        fprintf(stderr,"Forking Failed\n");

    if(ret == 0){
        if(bg == 1)
            childpid = getpid();

        if(arg[0] == NULL){
            fprintf(stderr,"Invalid Command\n");
        }
        
        else{
            flag = execvp(arg[0],arg);
            if(errno == 2)
                fprintf(stderr,"Invalid Command\n");
            else
                perror("Error Dobe");
            exit(0);
        }
    }
    else if(ret > 0){
        if(bg == 0){
            signal(SIGCHLD,SIG_IGN);
            waitpid(ret,NULL,0);
        }
        else{
            signal(SIGCHLD,bg_handler);
        }
    }
    if(flag != -1){
        if(bg == 1){
            strcpy(process_current[num_current],arg[0]);
            pid_current[num_current++] = childpid;
        }

        strcpy(process[num_process],arg[0]);
        pid[num_process++] = ret;
    }
}

/* Function for mainting pipe function. Counting the pipes the using file descripter 
and dup function to send input to output to input. */
void custom_pipe_function(char **arg,int len){

    int countpipe = 0, i, j, k = 0;
    int *partlen;
    int temp1, temp2;
    char ***var;

    partlen = (int *)calloc(size,sizeof(int));
    var = (char ***)calloc(size,sizeof(char **));

    for(i=0; i<size; i++)
        var[i] = (char **)calloc(size,sizeof(char *));

    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
            var[i][j] = (char *)calloc(size,sizeof(char));

    for(i=0; i<len; i++){
        if(strcmp(arg[i],"|") == 0){
            var[countpipe][k] = NULL;
            partlen[countpipe++] = k;
            k = 0;
        }
        else
            strcpy(var[countpipe][k++],arg[i]);
    }

    var[countpipe][k] = NULL;
    partlen[countpipe] = k;

    int a[countpipe][2];
    for(i=0; i<countpipe; i++){
        pipe(a[i]);
    }

    temp1 = dup(0);
    temp2 = dup(1);

    dup2(a[0][1],1);
    function_identifier(var[0],partlen[0]);

    for(i=1; i<countpipe; i++){
        close(a[i-1][1]);
        dup2(a[i-1][0],0);
        dup2(a[i][1],1);
        function_identifier(var[i],partlen[i]);
    }

    close(a[countpipe-1][1]);
    dup2(a[countpipe-1][0],0);
    dup2(temp2,1);
    function_identifier(var[countpipe],partlen[countpipe]);

    for(i=0; i<countpipe; i++)
        close(a[i][0]);

    dup2(temp1,0);
    free(partlen);
    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
            free(var[i][j]);

    for(i=0; i<size; i++)
        free(var[i]);

    free(var);
}

void redirection_func(char **arg,int len){
    int i, k = 0;
    int temp1, temp2;
    int fd1 = 0, fd2 = 0;
    char **temp;
    char input[200];
    char output[200];

    input[0] = '\0';
    output[0] = '\0';

    temp = (char **)calloc(size,sizeof(char *));
    for(i=0; i<size; i++)
        temp[i] = (char *)calloc(size,sizeof(char));

    for(i=0; i<len; i++){
        if(arg[i] == NULL)
            continue;
        else if(strcmp(arg[i],"<") == 0){
            if(len <= i+1){
                fprintf(stderr,"Invalid Command : unexpected token newline\n");
                return;
            }
            strcpy(input,arg[i+1]);
        }
        else if(strcmp(arg[i],">") == 0){
            if(len <= i+1){
                fprintf(stderr,"Invalid Command : unexpected token newline\n");
                return;
            }
            strcpy(output,arg[i+1]);
        }
    }

    for(i=0; i<len; i++){
        if(arg[i][0] == '<' || arg[i][0] == '>'){
            i++;
            continue;
        }
        if(arg[i] != NULL)
            strcpy(temp[k++],arg[i]);
    }
    temp[k] = NULL;

    if(input[0] != '\0')
        fd1 = open(input,O_RDONLY);

    if(output[0] != '\0')
        fd2 = open(output,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);

    if(fd1<0 || fd2<0){
        perror("Error");
        return;
    }
    
    temp1 = dup(0);
    temp2 = dup(1);
    
    if(fd1 > 0)
        dup2(fd1,0);
    
    if(fd2 > 0)
        dup2(fd2,1);
    
    function_identifier(temp,k);
    dup2(temp1,0);
    dup2(temp2,1);
    
    if(fd1 > 0)
        close(fd1);
    
    if(fd2 > 0)
        close(fd2);
    
    for(i=0; i<size; i++)
        free(temp[i]);
    
    free(temp);
}

/*Function adds redirection and piping together*/

void redirection_pipe_function(char **arg,int len){
    int countpipe = 0, i, j, k = 0;
    int *partlen;
    int temp1, temp2, fd1 = 0, fd2 = 0;
    char ***var;
    char input[50];
    char output[50];
    
    input[0]='\0';
    output[0]='\0';
    
    var = (char ***)calloc(size,sizeof(char **));
    
    for(i=0; i<size; i++)
        var[i] = (char **)calloc(size,sizeof(char *));
    
    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
            var[i][j] = (char *)calloc(size,sizeof(char));
    
    partlen = (int *)calloc(size,sizeof(int));

    for(i=0; i<len; i++){
        if(arg[i]==NULL)
            continue;
        else if(!strcmp(arg[i],">"))
            strcpy(output,arg[i+1]);
        else if(!strcmp(arg[i],"<"))
            strcpy(input,arg[i+1]);
    }
    
    if(input[0] != '\0')
        fd1=open(input,O_RDONLY);
    
    if(output[0] != '\0')
        fd2=open(output,O_CREAT|O_WRONLY|O_TRUNC,S_IRUSR|S_IWUSR);
    
    if(fd1 < 0 || fd2 < 0){
        perror("Error Dobe");
        return;
    }

    for(i=0; i<len; i++){
        if(arg[i] == NULL)
            continue;
        else if(strcmp(arg[i],"<") == 0 || strcmp(arg[i],">") == 0){
            i++;
            continue;
        }
        else if(strcmp(arg[i],"|") == 0){
            var[countpipe][k] = NULL;
            partlen[countpipe++] = k;
            k = 0;
        }
        else
            strcpy(var[countpipe][k++],arg[i]);
    }
    var[countpipe][k] = NULL;
    partlen[countpipe] = k;

    int a[countpipe][2];
    for(i=0; i<countpipe; i++){
        pipe(a[i]);
    }
    
    temp1 = dup(0);
    temp2 = dup(1);
    
    if(fd1 != 0)
        dup2(fd1,0);
    else
        dup2(temp1,0);

    dup2(a[0][1],1);
    function_identifier(var[0],partlen[0]);
    
    for(i=1; i<countpipe; i++){
        close(a[i-1][1]);
        dup2(a[i-1][0],0);
        dup2(a[i][1],1);
        function_identifier(var[i],partlen[i]);
    }
    dup2(a[countpipe-1][0],0);
    
    if(fd2 != 0)
        dup2(fd2,1);
    else
        dup2(temp2,1);
    
    close(a[countpipe-1][1]);
    function_identifier(var[countpipe],partlen[countpipe]);
    
    for(i=0; i<countpipe; i++){
        close(a[i][0]);
    }
    
    dup2(temp1,0);
    dup2(temp2,1);
    
    free(partlen);
    
    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
            free(var[i][j]);
    
    for(i=0; i<size; i++)
        free(var[i]);
    
    free(var);
}

/* Command Identifier for different categories of function created and callers for UNIX commands*/
void function_identifier(char **arg,int len)
{
    int i;
    int bg = 0;
    int pipe = 0;
    int redir = 0;
    
    if(arg[0] == NULL){
        fprintf(stderr,"Invalid Command\n");
    }
    
    for(i=0; i<len; i++)
        if((strcmp(arg[i],"<") == 0) || (strcmp(arg[i],">") == 0))
            redir = 1;
    
    for(i=0; i<len; i++)
        if(strcmp(arg[i],"|") == 0)
            pipe = 1;

    if(arg[len-1][strlen(arg[len-1])-1] == '&'){
        bg = 1;

        if(arg[len-1][0] == '&')
            arg[len-1] = NULL;
        else
            arg[len-1][strlen(arg[len-1])-1] = '\0';
    }

    if(pipe && !redir) {
        custom_pipe_function(arg,len);
    }
    else if(!pipe && redir){
        redirection_func(arg,len);
    }
    else if(pipe && redir){
        redirection_pipe_function(arg,len);
    }
    else if(strcmp(arg[0],"cd") == 0){
        cd(arg,len);
    }
    else if(strcmp(arg[0],"pid") == 0){
        pid_finder(arg,len);
    }
    else if(arg[0][0]=='h' && arg[0][1]=='i' && arg[0][2]=='s' && arg[0][3]=='t'){
        hist(arg,len);
    }
    else{
        other(arg,len,bg);
    }
}

int main(){
    signal(SIGINT,SIG_IGN);
    signal(SIGQUIT,SIG_IGN);
    signal(SIGTSTP,SIG_IGN);

    char host_name[size];
    char *user_name;
    char home_dir[size];
    char curr_dir[size];
    char *ptr_current_dir;
    int i;
    int j;

    user_name = getenv("USER");
    gethostname(host_name,size);
    pointer_to_home = getcwd(home_dir,size);
    shell_pid = getpid();

    history = (char ***)calloc(size,sizeof(char **));

    for(i=0; i<size; i++)
        history[i] = (char **)calloc(size,sizeof(char *));
    
    for(i=0; i<size; i++)
        for(j=0; j<size; j++)
            history[i][j] = (char *)calloc(size,sizeof(char));
    
    arglen = (int *)calloc(size,sizeof(int));

    process = (char **)calloc(size,sizeof(char *));
    
    for(i=0; i<size; i++)
        process[i] = (char *)calloc(size,sizeof(char));
    
    pid = (int*)calloc(size,sizeof(int));
    
    process_current = (char **)calloc(size,sizeof(char *));
    
    for(i=0; i<size; i++)
        process_current[i] = (char *)calloc(size,sizeof(char));
    
    pid_current = (int*)calloc(size,sizeof(int));
    
    num_process = 0;
    num_current = 0;

    while(1)
    {
        char inp[input_size];
        char temp[input_size];
        char *delim = NULL;
        char **arg;
        int len = 0;

        ptr_current_dir = getcwd(curr_dir,size);
        printf("%s@%s:",user_name,host_name);
        
        if(strstr(ptr_current_dir,pointer_to_home) == NULL)
            printf("%s",ptr_current_dir);
        else
            printf("~%s",ptr_current_dir + strlen(pointer_to_home));
        printf(">");
        
        inp[0] = '\0';
        temp[0] = '\0';

        arg = (char **)calloc(size,sizeof(char *));
        for(i=0; i<size; i++)
            arg[i] = (char *)calloc(size,sizeof(char));

        while(scanf("%[^\n]",inp) == EOF) {}
        getchar();
        inp[strlen(inp)] = '\0';
        strcpy(temp,inp);
        temp[strlen(temp)] = '\0';
        if(strcmp(inp,"exit") == 0)
            break;

        delim = strtok(inp," \t\n|<>");
        if(delim == NULL)
            continue;

        while(delim != NULL)
        {
            strcpy(arg[len++],delim);
            char *po = temp + (int)(delim-inp) + strlen(delim);
            while(*po==' ' || *po=='\t' || *po=='\n' || *po=='<' || *po=='>' || *po=='|')
            {
                if(*po=='<' || *po=='>' || *po=='|')
                {
                    arg[len][0] = *po;
                    arg[len++][1] = '\0';
                }
                po = po + 1;
            }
            delim = strtok(NULL," \t\n<>|");
        }
        
        arg[len] = NULL;
        history[num] = arg;
        arglen[num++] = len;
        function_identifier(arg,len);
        signal(SIGCHLD,bg_handler);
    }

    for(i=0;i<size;i++)
        free(process[i]);
    free(process);
    
    free(pid);
    
    for(i=0;i<size;i++)
        free(process_current[i]);
    free(process_current);
    
    free(pid_current);
    
    for(i=0;i<size;i++)
        for(j=0;j<size;j++)
            free(history[i][j]);
    for(i=0;i<size;i++)
        free(history[i]);

    printf("\n");
    return 0;
}

