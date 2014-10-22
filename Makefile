all: bin rshell

bin: 
	[ ! -d $bin ] && mkdir bin

rshell:	
	g++ -Wall -Werror -ansi -pedantic src/rshell.cpp -o bin/rshell
	