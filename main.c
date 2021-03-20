#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h> 
#include <malloc.h> 
#include <errno.h> 
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#define STARTTERMCNT 10
#define STARTPROCCNT 10
#define true 1
#define false 0
#define exitloop -1
///<summary>
///<param name="str">Строка, которую необходимо разделить на лексемы</param>
///<remarks>Входная строка изменяется в функции. Сохраняйте копию строки, если она будет нужна далее</remarks>
///<returns>Возвращает массив строк, разбитых по " \t". Последняя строка = NULL</returns>
///</summary>
char** strtoarr(char* str);
///<summary>
///<remarks>Функция пытается найти и выполнить встроенные возможности терминала: exit, cd, njobs, nkill</remarks>
///<param name="prm">Массив параметров</param>
///<returns>Возвращает:
///1, если ключевое слово найдено и выполнилось(кроме exit); 
///0, если ключевое слово не найдено; 
///-1, если необходимо завершить работу терминала(exit)</returns>
///</summary>
int    shexec  (char** prm);
int    execute (char** args,  int isBackground);
struct bgproc{
	pid_t pid;           //process ID
	char name[256]={0};  //максимальная длина имени файла в Linux - 255
};
/*
int flagkilljobs=0;
signal(SIGINT,intsignal);
void intsignal(){
	flagkilljobs=1;
	return 0;
}
while(1){
	...
	if(flagkilljobs==1){
		nkill(jobs);
	}
}
*/
/*
int flagAvil=0;
signal(SIGINT,intsignal);
void intsignal(){
	if(flagAvil){
		nkill(jobs)
	}
	else
	{
		flagkilljobs=1;
	}
}
while(1){
	...
	flagAvil=0;
	...
	if(flagkilljobs=1){
		nkill(jobs);
		flagkilljobs=0;
	}
	flagAvil=1
}
*/
int main()
{
	char* 	line         = NULL;			//строка для ввода
    size_t 	size         = 0;				//размер буфера
	int     isBackground = false;			//флаг фонового режима запуска
	char**  args;							//массив аргументов
	int 	maxproccnt   = STARTPROCCNT;	//максимальное количество процессов
	int 	curproccnt   = 0;				//количество процессов в фоне
	bgproc* jobs         = (bgproc)malloc(sizeof(bgproc) * maxproccnt);//массив процессов в фоне
	while (true){
		printf("> ");
	
	int str_len = getline(&line, &size, stdin);
	if(str_len == -1){
		perror("error\n");
		exit(0);
	}
	if(str_len==1){
		continue;
	}

	if (line[str_len-1]=='\n'){
		line[str_len-1]='\0';
	}
    args = strtoarr(line);
	int ans = shexec(args);
	if      (ans == true){
		free(args);
		continue;
	}
	if      (ans == exitloop){
		break;
	}
	for(int i=0; args[i] != NULL; i++){
		if(strcmp(args[i],"&")==0){
			isBackground = true;
			args[i] = NULL;
		}
	}
	if(execute(args, isBackground) == false){
		break;
	}
	isBackground=false;
	free(args);
	}//end main loop
    free(line);
	free(args);

    return 0;
}// END MAIN

char** strtoarr(char* str)
{
	size_t	termcnt=STARTTERMCNT;                           //количество лексем
	char** args = (char**)malloc(sizeof(char*) * termcnt);  //массив аргументов
	char* sep   = " \t";								    //разделитель для strtok
    char*  istr = strtok (str, sep);						
	int ptr     = 0;									    //номер последнего аргумента
	char** tempargs;										//временное хранилище для массива аргументов
    for (ptr=0; istr != NULL; ptr++)
    {
		
	    args[ptr]=istr;
		// если не хватает памяти, то увеличить ее в 2 раза
	    if(ptr>termcnt-1){
			char** tempargs = args;
			args = realloc(args, sizeof(char*)*termcnt*2);
			if(args == NULL){
				perror("realloc");
				exit(0);
			}
	    }
        istr = strtok (NULL, sep);
    }
	args[ptr]=NULL;
	return args;
}
int shexec(char** prm){
	if(strcmp(prm[0], "exit") == 0){
		printf("bye\n");
		return exitloop;
	}
	if(strcmp(prm[0], "cd") == 0){
		if(chdir(prm[1]) == -1){
			perror(prm[1]);
		}
		return true;
	}
	return false;
}
int execute(char** args, int isBackground){
	    pid_t 	pidChild;
	    if((pidChild = fork()) == -1){
		perror("ERROR ");
	}
    if(pidChild == 0)//if child
    {
		
		if (execvp(args[0], args) == -1) {
            perror(args[0]);
			return false;
		}
    }
    else//if parent
    {
		if(isBackground){
			printf("[%d: %d]\n", 1, pidChild);//TODO
			
		}
		else{
			waitpid(pidChild, 0,0);
			return true;
		}
    }
}