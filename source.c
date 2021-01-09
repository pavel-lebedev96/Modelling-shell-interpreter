#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#define BUFSIZE 1024
#define MAX_ARGS 10

//создание процесса и вызов exec с заданными аргументами
int new_process(char *args[])
{
    int status;
    /*создание процесса*/
    switch (fork())
    {
    case -1:
        perror("Cannot fork");
		return -1;
    case 0:
        /*дочерний процесс*/
        execvp(args[0], args);
        perror("Cannot exec");
		exit(1);
        break;
    default:
        /*родительский процесс*/
        wait(&status);
        return(status);
    }
}

//разбиение строки на аргументы и заполнение массива аргументов args
void get_args(char* str, char* args[])
{
	int i = 0;
	args[i] = strtok(str, " ");
	while (i < MAX_ARGS && args[i++])
		args[i] = strtok(NULL, " ");
}

int main(int argc, char* argv[])
{
	//args - массив аргументов, str - строка с командой и аргументами
	char* args[MAX_ARGS], str[BUFSIZE];
	
	printf("Enter \"exit\" to close the program\n");
	for (;;)
	{
		//считывание строки
		gets(str);
		if (strcmp(str, "exit") == 0)
			return 0;
		
		//получение массива аргументов
		get_args(str, args);
		
		//создание процесса
        printf("The exit status: %d\n", new_process(args));
	}
}
