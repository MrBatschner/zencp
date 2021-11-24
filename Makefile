#!/usr/bin/make -f

CFLAGS=-Wall -O -g
CXXFLAGS=${CFLAGS}
LDLIBS=-lid3 -lnjb -lstdc++
CC=gcc
CXX=g++

OBJECTS=misc.o list.o id3.o id3_header.o player.o tracklist.o zencp.o

all:	zencp

zencp:	${OBJECTS}

# the C section
misc.o:		misc.c misc.h
list.o:		list.c list.h
id3.o:		id3.c id3.h
player.o:	player.c player.h
zencp.o:	zencp.c zencp.h

# the C++ section
id3_header.o:	id3_header.cpp id3_header.h

.PHONY:	clean
clean:
	rm -f *.o zencp
