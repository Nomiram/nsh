#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h> 
#include <malloc.h> 
#include <errno.h> 
#include <string.h>
char** strtoarr(char* str);
int main()
{
    pid_t 	pidChild;
	char* 	line = NULL;
    size_t 	size = 0;
	size_t	termcnt=10;//количество лексем
	int s = 1;
	char** args = (char**)malloc(sizeof(char*) * termcnt);
	while (s){
		printf("> ");
	
	int str_len = getline(&line, &size, stdin);
	if(str_len==-1){
		printf("error\n");
	}
	
	if (line[str_len-1]=='\n'){
		line[str_len-1]='\0';
	}
    char* sep=" \t";
    char*  istr = strtok (line,sep);
	int ptr=0;
    for (ptr=0; istr != NULL;ptr++)
    {
		
	    args[ptr]=istr;
		// printf("PTR = %p\n",istr);
	    if(ptr>termcnt-1){
			args=realloc(args,sizeof(char*)*termcnt*2);
	    }
        istr = strtok (NULL,sep);
    }
	args[ptr]=NULL;
	if(strcmp(args[0],"exit")==0){
		break;
	}
    if((pidChild = fork())==-1){
		perror("ERROR ");
	}
    if(pidChild == 0)//if child
    {
		
		if (execvp(args[0], args) == -1) {
            perror("ERROR ");
		}
    }
    else//if parent
    {
		waitpid(pidChild, 0,0);
    }

	}//end main loop
    free(line);
	free(args);

    return 0;
}
char** strtoarr(char* str)
{
	return NULL;
}
