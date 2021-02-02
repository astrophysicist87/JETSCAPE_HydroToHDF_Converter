// read me
//This file is the seperate interpolation function. Contains several sub functions.
// 1) readFromFile(): to read in the hydro's files. change the input file name like "data1_ev1.dat" to read in different files
// 2) sortcol(): sort the vector based on the distance, we need to find the 8 closetest points near the target one
// 3) interpolation(): do the interpolation for a single target point based on the 8 points.
// 4) in main(),change the constant 50205 to the number of rows if you change the input file. For Jaki's hydro files, it is always 50205
// 5) "points" is a 2D vector stores all the info we need.
// points[i][j]:
// j = 0/1 -> x/y coordinate; j = 2 -> energy density; j = 3 -> Temperature in terms of GeV; j = 4/5 -> velocity in x/y; j = 6 -> distance


#include <fstream>
#include <iostream>
#include <assert.h>
#include <vector>
#include <math.h>
#include <algorithm>

#include "interpolate_hydro.h"
#include "JETSCAPE_output_to_HDF.h"

using namespace std;

int main(int argc, char ** argv)
{
	if ( argc < 2 )
	{
		cerr << "Usage: ./interpolate_hydro [files to interpolate]" << endl;
		cerr << "E.g.:  ./interpolate_hydro /path/to/results/sveprofile_{1..255}_ev0.dat" << endl;
		return (8);
	}

	// set grid for interpolation
	//cout << endl << " - Generating grid for interpolation" << endl;
	const double dx = 0.288, dy = 0.288;
	//const int xGridSize = 107, yGridSize = 107;
	const int xGridSize = 5, yGridSize = 5;
	const double xmin = -dx*(xGridSize-1.0)/2.0, ymin = -dy*(yGridSize-1.0)/2.0;
	vector<double> xGrid(xGridSize), yGrid(yGridSize);
	generate(xGrid.begin(), xGrid.end(), [n = 0, &xmin, &dx] () mutable { return xmin + dx * n++; });
	generate(yGrid.begin(), yGrid.end(), [n = 0, &ymin, &dy] () mutable { return ymin + dy * n++; });

	// set up HDF5 stuff
	const H5std_string FILE_NAME("output.h5");
	string GROUPEVENT_NAME = "/Event";
	const double DX = dx, DY = dy, Tau0 = 0.048, TauFS = 1.2, dTau = 0.072;
	const int OutputViscousFlag = 1,
			  XH = (xGridSize-1)/2, XL = -(xGridSize-1)/2,
			  YH = (yGridSize-1)/2, YL = -(yGridSize-1)/2;

	try
    {
		Exception::dontPrint();
	
		//----------------------------------------------------------------------
		// set up HDF file and required attributes
		H5File file(FILE_NAME, H5F_ACC_TRUNC);
		Group groupEvent(file.createGroup(GROUPEVENT_NAME.c_str()));
		output_attributes( groupEvent, DX, DY, OutputViscousFlag, Tau0, TauFS,
							XH, XL, YH, YL, dTau );

		//----------------------------------------------------------------------
		// loop over files and read in one at a time
		for (int iArg = 1; iArg < argc; iArg++)
		{
			//------------------------------------------------------------------
			// set filename and load data
			string filename = argv[iArg];
			cout << " - Reading in " << filename << endl;
			
			//------------------------------------------------------------------
			// outputGrid contains data from filename, interpolated to grid
			// defined by xGrid (x) yGrid
			vector<vector<double> > outputGrid;
			interpolate_hydro_driver( filename, outputGrid, xGrid, yGrid );		

			//------------------------------------------------------------------
			// send outputGrid to HDF file
			const int width = 4;
			string FRAME_NAME = GROUPEVENT_NAME + "/Frame_"
								+ get_zero_padded_int( iArg-1, width );
			output_dataset( file, FRAME_NAME, Tau0 + dTau*(iArg-1.0),
							outputGrid, xGridSize, yGridSize, 6 );

			//cout << " - Finished interpolating " << filename << endl;
		}

	}

	// catch any errors
    catch(FileIException error)
    {
		error.printError();
		return (-1);
    }

    catch(DataSetIException error)
    {
		error.printError();
		return (-1);
    }

    catch(DataSpaceIException error)
    {
		error.printError();
		return (-1);
    }

	return 0;
}
