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
CFLAGS= -g -std=c++14 -lgsl -lgslcblas -lm

# Various directories and definitions
RM          =   rm -f
O           =   .o
LDFLAGS     =   
INCFLAGS    =   
SYSTEMFILES =   $(SRCGNU)

# --------------- Files involved ------------------

MAIN		 =	interpolate_hydro
MAIN2		 =	interpolate_hydro_delaunay

MAINSRC      =  interpolate_hydro.cpp
MAINSRC2     =  interpolate_hydro_delaunay.cpp \
				delaunay/table_delaunay.cpp

INC			 = 	interpolate_hydro.h \
				output.h \
				JETSCAPE_output_to_HDF.h
INC2		 = 	interpolate_hydro_delaunay.h \
				delaunay/table_delaunay.h \
				output.h

# -------------------------------------------------

TARGET		=	$(MAIN)
TARGET2		=	$(MAIN2)

# --------------- Pattern rules -------------------

$(TARGET):
	$(CC) $(MAINSRC) -o $(TARGET) $(CFLAGS) $(INCFLAGS) $(LDFLAGS)

$(TARGET2):
	$(CC) $(MAINSRC2) -o $(TARGET2) $(CFLAGS) $(INCFLAGS) $(LDFLAGS)

# -------------------------------------------------

.PHONY:		all help distclean

all:		$(TARGET) $(TARGET2)

help:
		@grep '^##' GNUmakefile

distclean:	
		-rm $(TARGET)
		-rm $(TARGET2)

# --------------- Dependencies -------------------
interpolate_hydro.cpp:             interpolate_hydro.h output.h JETSCAPE_output_to_HDF.h
interpolate_hydro_delaunay.cpp:    interpolate_hydro_delaunay.h output.h \
                                   delaunay/table_delaunay.h
delaunay/table_delaunay.cpp:       delaunay/table_delaunay.h
