# ==============================================================================
#  Purpose: Makefile for hydrodynamic interpolation and HDF converter
#  Author:  Christopher Plumberg
#  Date:    January 19, 2021
# ==============================================================================
##
##  Environments :	MAIN	= 	main sourcefile	
##
##  Usage : 	(g)make	[all]		compile the whole project		
##			distclean	remove all binaries
##  

# HDF5 flags
export HDF5_CXX=$(which g++)
export HDF5_CXXLINKER=$(which g++)
export HDF5_CLINKER=$(which g++)

# Set compiler and flags
CC := h5c++
CFLAGS= -std=c++14 -lgsl -lgslcblas -lm

# Various directories and definitions
RM          =   rm -f
O           =   .o
LDFLAGS     =   
INCFLAGS    =   
SYSTEMFILES =   $(SRCGNU)

# --------------- Files involved ------------------

MAIN		 =	interpolate_hydro

MAINSRC      =  interpolate_hydro.cpp

INC			 = 	interpolate_hydro.h \
				output.h

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
interpolate_hydro.cpp:         interpolate_hydro.h output.h