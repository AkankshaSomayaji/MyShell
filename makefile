username = $(shell whoami)
SRCDIR = src
files = $(SRCDIR)/main.c $(SRCDIR)/tokens.c $(SRCDIR)/commands.c
opflag = -o shell

all: run

run: compile
	./shell

debugrun: compile
	valgrind --track-origins=yes ./shell

compile:
	gcc -Wall -g $(files) $(opflag)

