username = $(shell whoami)
SRCDIR = src
files = $(SRCDIR)/main.c $(SRCDIR)/tokens.c
opflag = -o shell

all: run

run: compile
	./shell

compile:
	gcc -Wall -g $(files) $(opflag)