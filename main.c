#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h> 
#include <malloc.h> 
#include <errno.h> 
// #define NULL 0
int main()
{
    pid_t 	var1;
	char* 	line = NULL;
    size_t 	size = 0;
    printf("> ");
	
	int str_len = getline(&line, &size, stdin);
	if(str_len==-1){
		printf("error\n");
	}
	char** args = malloc(sizeof(char*));
	if (line[str_len-1]=='\n'){
		line[str_len-1]=='\0';
		printf("nullllll\n");
	}
	// args [0]= line;
	args [0]= "/bin/ps";
	printf("%d %s\n",str_len,args[0]);
	// args [1]= "\0";
    var1 = fork();
    if(var1 == 0)//if child
    {
    // execvp(args[0], args);
	        if (execvp(args[0], args) == -1) {
            printf("[ERROR] Couldn't execute unknown command!\n");
        }
    // execv("/bin/ps", "/bin/ps", NULL);
    }
    else//if parent
    {
		
    }
    wait(0);
    return 0;
}
