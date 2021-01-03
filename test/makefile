# -*- MakeFile -*-
# NAME: Copyright (C) 2021, Biren Patel
# DESC: build system for the SCIPACK unit and integration tests
# NOTE: this makefile must be triggered recursively from the root
# LICS: MIT License

#------------------------------------------------------------------------------#
# Compiler
#------------------------------------------------------------------------------#

CC = gcc

CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -Wnull-dereference
CFLAGS += -Wdouble-promotion -Wconversion -Wcast-qual -Wpacked -Wpadded
CFLAGS += -m64 -march=native -mrdrnd -msse2 -O0
CFLAGS += -ggdb
CFLAGS += -I../include/ -I../extern/unity/include

LDFLAGS = -L../build/lib

#------------------------------------------------------------------------------#
# Setup
#------------------------------------------------------------------------------#

.PHONY : all clean

vpath %.a ../build/lib
vpath %.so ../build/lib

vpath %.h ../include/
vpath %.h ../extern/unity/include

#test dir exactly mirrors the src dir
vpath %.c ../src/
vpath %.c ../src/random ./random
vpath %.c ../src/timing ./timing

#------------------------------------------------------------------------------#
# Targets
#------------------------------------------------------------------------------#

.PHONY : random
module_a := test_generator_sisd

.PHONY : timing
module_b := test_timer

executables = $(module_a) $(module_b)

#direct copy of objects_raw variable in root makefile
objects = generator_sisd.o
objects += timer.o

#stack the test object file to the copy
objects += test_generator_sisd.o
objects += test_timer.o

#------------------------------------------------------------------------------#
# Build All Tests
#------------------------------------------------------------------------------#

all : $(executables)

#------------------------------------------------------------------------------#
# Module A: psuedo random number generation
#------------------------------------------------------------------------------#

random: $(module_a)

#random sisd submodule
test_generator_sisd : test_generator_sisd.o generator_sisd.o
	$(CC) -o $@ $^ $(LDFLAGS) -lunity

test_generator_sisd.o : test_generator_sisd.c generator_sisd.h unity.h
	$(CC) $(CFLAGS) -c -o $@ $<

generator_sisd.o : generator_sisd.h

#------------------------------------------------------------------------------#
# Module B: high resolution timing
#------------------------------------------------------------------------------#

timing: $(module_b)

#timer submodule
test_timer : test_timer.o timer.o
	$(CC) -o $@ $^ $(LDFLAGS) -lunity

test_timer.o : test_timer.c timer.h unity.h
	$(CC) $(CFLAGS) -c -o $@ $<

timer.o : timer.h
