#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h> 
#include <malloc.h> 
#include <errno.h> 
#include <string.h>
int main()
{
    pid_t 	pidChild;
	char* 	line = NULL;
    size_t 	size = 0;
    printf("> ");
	
	int str_len = getline(&line, &size, stdin);
	if(str_len==-1){
		printf("error\n");
	}
	char** args = malloc(sizeof(char*));
	if (line[str_len-1]=='\n'){
		line[str_len-1]='\0';
	}
	args [0]= line;
    pidChild = fork();
    if(pidChild == 0)//if child
    {
		if (execvp(args[0], args) == -1) {
            perror("ERROR ");
		}
    }
    else//if parent
    {
		
    }
    wait(0);
    return 0;
}
