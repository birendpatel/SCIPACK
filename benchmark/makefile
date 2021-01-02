# -*- MakeFile -*-
# NAME: Copyright (C) 2020, Biren Patel
# DESC: build system for the SCIPACK benchmarks, invoke recursively from root
# LICS: MIT License

CC = gcc

CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -Wnull-dereference
CFLAGS += -Wdouble-promotion -Wconversion -Wcast-qual -Wpacked -Wpadded
CFLAGS += -m64 -march=native -O2
CFLAGS += -I../include/

#------------------------------------------------------------------------------#
# Setup
#------------------------------------------------------------------------------#

BLDDIR := ../build/lib
HEADER := ../include

LDFLAGS = -L$(BLDDIR)

vpath %.a $(BLDDIR)
vpath %.h $(HEADER)

#------------------------------------------------------------------------------#
# Build
#------------------------------------------------------------------------------#

benchmarks : benchmarks.c timer.h
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lscipack