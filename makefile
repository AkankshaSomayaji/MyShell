username = $(shell whoami)
files = main.c tokens.c
opflag = -o shell

all: run

run: compile
	./shell

compile:
	gcc -Wall -g $(files) $(opflag)

