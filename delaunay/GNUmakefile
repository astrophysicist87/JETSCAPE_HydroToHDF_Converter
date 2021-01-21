# ==============================================================================
#  Purpose: Makefile for Delaunay interpolator
#  Author:  Christopher Plumberg
#  Date:    January 20, 2021
# ==============================================================================
##
##  Environments :	MAIN	= 	main sourcefile	
##
##  Usage : 	(g)make	[all]		compile the whole project		
##			distclean	remove all binaries
##  

# Set compiler and flags
CC := g++
CFLAGS= -g -std=c++14 -lgsl -lgslcblas -lm

# Various directories and definitions
RM          =   rm -f
O           =   .o
LDFLAGS     =   
INCFLAGS    =   
SYSTEMFILES =   $(SRCGNU)

# --------------- Files involved ------------------

MAIN		 =	delaunay_interp_2d

MAINSRC      =  delaunay_interp_2d.cpp \
				table_delaunay.cpp

INC			 = 	table_delaunay.h

# -------------------------------------------------

TARGET		=	$(MAIN)

# --------------- Pattern rules -------------------

$(TARGET):
	$(CC) $(MAINSRC) -o $(TARGET) $(CFLAGS) $(INCFLAGS)  $(LDFLAGS)

# -------------------------------------------------

.PHONY:		all help distclean

all:		$(TARGET)

help:
		@grep '^##' GNUmakefile

distclean:	
		-rm $(TARGET)

# --------------- Dependencies -------------------
table_delaunay.cpp:         	table_delaunay.h
delaunay_interp_2d.cpp:         table_delaunay.h
