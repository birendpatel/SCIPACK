# -*- MakeFile
# NAME: Copyright (C) 2020, Biren Patel
# DESC: build a dynamic lib for unity
# NOTE: this build invalidates the setUp() and tearDown() functionality
# LICS: MIT License

CC = gcc

#includes
CFLAGS += -I./include/

#------------------------------------------------------------------------------#
# Setup
#------------------------------------------------------------------------------#

vpath %.c ./src/
vpath %.h ./include/

.PHONY : all dirs clean

#------------------------------------------------------------------------------#
# Build
#------------------------------------------------------------------------------#

all : libunity.a
	rm *.o

libunity.a : unity.o
	$(AR) $(ARFLAGS) -o $@ $?

unity.o : unity.c unity.h
	$(CC) $(CFLAGS) -c -o $@ $<

#------------------------------------------------------------------------------#
# Build
#------------------------------------------------------------------------------#

clean :
	rm *.so