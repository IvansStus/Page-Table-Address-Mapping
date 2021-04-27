# Ivan Stus - 821201908
# Kenny Kieu - 819180993
# CS 570-05
# Spring 2021
# Assignment #3 - Multi-Level Page Table
# Description - XXX

# CXX Make variable for compiler
###CXX=gcc
# Make variable for compiler options
#    -std=c++11  C/C++ variant to use, e.g. C++ 2011
#    -g          include information for symbolic debugger e.g. gdb 
#CXXFLAGS=-std=c++11 -g

# Rules format:
# target : dependency1 dependency2 ... dependencyN
#     Command to make target, uses default rules if not specified

# First target is the one executed if you just type make
# make target specifies a specific target
# $^ is an example of a special variable.  It substitutes all dependencies
####pagetable	:	driver.c	structFunctions.c	byu.c	outputhelp.c
	####$(CXX)	$(CXXFLAGS) -c pagetable -lm	$^


####clean	:
	###rm pagetable

CC=	gcc

CFLAGS=	-c -Wall

pagetable: driver.o structFunctions.o byu.o	outputhelp.o
	$(CC)	driver.o	structFunctions.o	byu.o	outputhelp.o	$(LFLAGS)	-o	pagetable	-lm

driver.o: driver.c
	$(CC) $(CFLAGS) driver.c

structFunctions.o: structFunctions.c
	$(CC)	$(CFLAGS)	structFunctions.c

byu.o: byu.c
	$(CC) $(CFLAGS)	byu.c

outputhelp.o: outputhelp.c
	$(CC) $(CFLAGS)	outputhelp.c

clean:
	$(RM) *.o pagetable