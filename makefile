#Makefile for Modelling-shell-interpreter
#Создание исполняемого модуля
binary: source.c
	gcc -o program source.c
#End Makefile