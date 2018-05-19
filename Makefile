CPP=g++
CC=gcc
LEX=flex
YACC=bison
LD=gcc

all:	ckarp

ckarp:	def.tab.o lex.yy.o
	$(CPP) lex.yy.o def.tab.o -o ckarp -ll -std=c++11

lex.yy.o:	lex.yy.c
	$(CC) -c lex.yy.c

lex.yy.c: z3.l
	$(LEX) z3.l

def.tab.o:	def.tab.cc
	$(CPP) -c def.tab.cc -std=c++11

def.tab.cc:	def.yy
	$(YACC) -d def.yy

clean:
	rm *.o ckarp
