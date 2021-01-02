# -*- MakeFile -*-
# NAME: Copyright (C) 2021, Biren Patel
# DESC: build system for the SCIPACK static library
# LICS: MIT License

CC = gcc

CFLAGS = -Wall -Wextra -Werror -pedantic -std=c99 -Wnull-dereference
CFLAGS += -Wdouble-promotion -Wconversion -Wcast-qual -Wpacked -Wpadded
CFLAGS += -m64 -march=native -mrdrnd -msse2 -O2
CFLAGS += -I./include/

#------------------------------------------------------------------------------#
# Setup
#------------------------------------------------------------------------------#

.PHONY : all directories tests benchmarks clean clean_tests
.PHONY : unity
dependencies := directories unity

BLDDIR := ./build/
OBJDIR := ./build/obj/
LIBDIR := ./build/lib/

vpath %.h ./include/
vpath %.a $(LIBDIR)
vpath $.so $(LIBDIR)
vpath %.o $(OBJDIR)

vpath %.c ./src
vpath %.c ./src/random
vpath %.c ./src/timing

objects_raw := random_sisd.o timer.o
objects := $(addprefix $(OBJDIR), $(objects_raw))

#------------------------------------------------------------------------------#
# Build SCIPACK
#------------------------------------------------------------------------------#

all : $(dependencies) scipack

directories :
	mkdir -p $(BLDDIR)
	mkdir -p $(OBJDIR)
	mkdir -p $(LIBDIR)

unity :
	$(MAKE) -C ./extern/unity
	mv ./extern/unity/libunity.a $(LIBDIR)

scipack : $(LIBDIR)libscipack.a

$(LIBDIR)libscipack.a : $(objects)
	$(AR) $(ARFLAGS) $@ $?

$(OBJDIR)random_sisd.o : random_sisd.c random_sisd.h
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJDIR)timer.o : timer.c timer.h
	$(CC) $(CFLAGS) -c -o $@ $<

#------------------------------------------------------------------------------#
# Build Tests
#------------------------------------------------------------------------------#

#override by user for an individual test or a module group of tests
only = all

tests: $(dependencies)
	$(MAKE) $(only) -C ./test/
	mv ./test/*.exe ./build

#------------------------------------------------------------------------------#
# Build Benchmarks
#------------------------------------------------------------------------------#

benchmarks : $(dependencies) scipack
	$(MAKE) -C ./benchmark/
	mv ./benchmark/benchmarks ./build

#------------------------------------------------------------------------------#
# Clean
#------------------------------------------------------------------------------#

clean:
	rm -rf $(BLDDIR)

clean_tests:
	rm ./test/*.o
