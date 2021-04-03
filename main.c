/*
Вопросы:
1. Variable 'flagAvail' is reassigned a value before the old one has been used if variable is no semaphore variable.
*/
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
///<para>Входная строка изменяется в функции. Сохраняйте копию строки, если она будет нужна далее</para>
///</summary>
///<param name="str">Строка, которую необходимо разделить на лексемы</param>
///<returns>Возвращает массив строк, разбитых по " \t". Последняя строка = NULL</returns>
char** strtoarr (char* str);
///<summary>
///<para>Функция пытается найти и выполнить встроенные возможности терминала: exit, cd, njobs, nkill итд</para>
///</summary>
///<param name="prm">Массив параметров</param>
///<returns>
///1, если ключевое слово найдено и выполнилось(кроме exit); 
///0, если ключевое слово не найдено; 
///-1, если необходимо завершить работу терминала(exit)
///</returns>
int    shexec   (char** prm);
///<summary>
///<para>Функция выполнить программу, путь к которой задается в первом аргументе</para>
///</summary>
///<param name="args">Массив параметров</param>
///<param name="isBackground">Должен ли процесс запустится в фоне (1-да,0-нет)</param>
///<returns>
///nothing - при успехе
///false - при неудаче
///</returns>
int    execute  (char** args,  int isBackground);
///<summary>
///<para>Обработчик прерывания SIGTERM</para>
///</summary>
///<returns>nothing</returns>
void   intsignal(int sig);
///<summary>
///<para>Обработчик прерывания SIGCHLD</para>
///</summary>
///<returns>nothing</returns>
void   endedprocess(int sig);
///<summary>
///<para>Показать список команд в консоль</para>
///</summary>
///<returns>nothing</returns>
void   ShowHelp();
int killjobs();
struct bgproc{
	int status;      //1 if working, 0 if ended
	pid_t pid;       //process ID
	char name[256];  //максимальная длина имени файла в Linux - 255
};
/*ГЛОБАЛЬНЫЕ ПЕРЕМЕННЫЕ*/
int flagAvail = 1;
int flagKill  = 0;
int 	maxproccnt   = STARTPROCCNT;	//максимальное количество процессов
int 	curproccnt   = 0;				//количество процессов в фоне
struct bgproc* jobs;                    //массив процессов в фоне
/*ТОЧКА ВХОДА В ПРОГРАММУ*/
int main()
{
	jobs  = malloc(sizeof(struct bgproc) * maxproccnt);
	signal(SIGINT,intsignal);
	signal(SIGTERM,intsignal);
	signal(SIGCHLD,endedprocess);
	char* 	line         = NULL;			//строка для ввода
    size_t 	size         = 0;				//размер буфера
	int     isBackground = false;			//флаг фонового режима запуска
	char**  args;							//массив аргументов
	ShowHelp();
	while (true){
		printf("> ");
	
	int str_len = getline(&line, &size, stdin);
	if(str_len == -1){
		printf("\n");
		continue;
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
	free(jobs);
    return 0;
}// END MAIN

void intsignal(int sig){
	printf("int signal\n");
	if(flagAvail==1){
		killjobs();
	}else{
		printf("Some important work doing now. \nTry later\n");
	}
}

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
	if(strcmp(prm[0], "njobs") == 0){
		flagAvail=0;
			printf("jobs:\n");
		if(curproccnt==0){printf("no bg jobs\n");}
		for(int i=0; i<curproccnt;i++){
			printf("  [%i:%i]\t%s\t%s\n",i,jobs[i].pid,jobs[i].name,jobs[i].status?"working":"ended");
		}
		flagAvail=1;
		return true;
	}
	if(strcmp(prm[0], "nhelp") == 0){
		ShowHelp();
		return true;
	}
	if(strcmp(prm[0], "nkill") == 0){
		if(flagAvail == 1){
			killjobs();
		}
		else
		{
			printf("error\n");
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
			printf("bg [%d: %d]\n", curproccnt, pidChild);
			flagAvail=0;
			jobs[curproccnt].pid = pidChild;
			memcpy(jobs[curproccnt].name, args[0], strlen(args[0])+1);
			jobs[curproccnt].status = 1;
			curproccnt++;
			flagAvail = 1;
		}
		else{
			waitpid(pidChild, 0, 0);
			return true;
		}
    }
}//end execute
int killjobs(){
		flagAvail=0;
		
			printf("kill jobs\n");
		for(int i=0; i<curproccnt;i++){
			printf("  [%i:%i]\t%s\n",i,jobs[i].pid,jobs[i].name);
			if(jobs[i].status == 1)
			{
				if(kill(jobs[i].pid, SIGKILL) == -1){
					//perror("Cannot kill process: ");
				}
			}
			wait(&jobs[i].pid);
		}
		curproccnt = 0;
		flagAvail=1;
		return true;
}
void endedprocess(int sig){
	pid_t pid;
    int status;
    while((pid = waitpid(-1,&status,WNOHANG)) > 0){   
	if(WIFEXITED(status)){
		for(int i = 0; i < curproccnt;i++){
			if(jobs[i].pid==pid){
				jobs[i].status=0;
			}
		}
	}
    }

}
void ShowHelp(){
	printf("\
		COMMANDS\n\
cd      - Change directory\n\
njobs   - Show background jobs\n\
nhelp   - Show this\n\
nkill   - Kill all BG jobs (the same that Ctrl+C)\n\
exit    - Exit program\n\
");
return;
}