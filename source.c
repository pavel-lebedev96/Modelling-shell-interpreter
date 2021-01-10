#include <sys/types.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

//константы
#define BUFSIZE 1024
#define MAX_ARGS 10
#define MAX_COM_COUNT 2

// Вывод сообщения об ошибке и завершение процесса
int error(char* s)
{
    perror(s);
    exit(1);
}

/*Перенаправление ввода/вывода и вызов exec
args - аргументы программы
fd_in - дескриптор для ввода
fd_out - дескриптор для вывода*/
void new_process(char* args[], int fd_in, int fd_out)
{
    /*перенаправление ввода*/
    if (fd_in != 0)
    {
        dup2(fd_in, 0);
        close(fd_in);
    }
    
    /*перенапрвление вывода*/
    if (fd_out != 1)
    {
        dup2(fd_out, 1);
        close(fd_out);
    }
    
    execvp(args[0], args);
    error("Cannot exec");
}

/*конвейер
args1 - список аргументов первой команды
args2 - список аргументов второй команды (может быть пустым) */
int join(char* args1[], char* args2[])
{
    int status;
    /*создание процесса*/
    switch (fork())
    {
    case -1:
        perror("Cannot fork");
        return 1;
    case 0:
        /*1-й дочерний процесс*/
        //если вторая команда отсутствует
        if (args2[0] == NULL)
        {
            execvp(args1[0], args1);
            error("Cannot exec");
        }
        break;
    default:
        /*главный родительский процесс*/
        wait(&status);
        return(status);
    }

    int p[2];
    /*создание канала*/
    if (pipe(p) == -1)
        error("Cannot pipe");

    switch (fork())
    {
    case -1:
        error("Cannot fork");
    case 0:
        /*2-й дочерний процесс*/
        close(p[0]);
        //создание процесса: ввод - стандартный, вывод - канал
        new_process(args1, 0, p[1]);
    default:
        /*родительский процесс (1-й дочерний процесс)*/
        close(p[1]);
        //создание процесса: ввод - канал, вывод - стандартный
        new_process(args2, p[0], 1);
        break;
    }
}

//разбиение строки на лексемы и заполнение массива лексем tokens
void strip(char* str, char* tokens[], const char *delim, int max_count)
{
	int i = 0;
	tokens[i] = strtok(str, delim);
	while (i < max_count && tokens[i++])
		tokens[i] = strtok(NULL, delim);
}

//главная подпрограмма
int main(int argc, char* argv[])
{
	/*args1 и args2 - массивы аргументов для первой и второй команд, соответсвенно,
    str - строка с командами и их аргументами,
    com_str - разделенные строки команд*/
    char* args1[MAX_ARGS], * args2[MAX_ARGS], str[BUFSIZE],
        * com_str[MAX_COM_COUNT];
	
    printf("Enter \"exit\" to close the program\n");
	for (;;)
	{	
        //считывание строки
		gets(str);
		if (strcmp(str, "exit") == 0)
			return 0;

        //разделение строки на две команды, если присутствует вторая команда
        strip(str, com_str, "|", MAX_COM_COUNT);

        //разделение строки на аргументы для первой команды
        strip(com_str[0], args1, " ", MAX_ARGS);
        //разделение строки на аргументы для первой команды
        strip(com_str[1], args2, " ", MAX_ARGS);

		//запуск процесса, если одна команда, или конвейера, если две, и вывод статуса
		printf("The exit status: %d\n", join(args1, args2));
	}
}
