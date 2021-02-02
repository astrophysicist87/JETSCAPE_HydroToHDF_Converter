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
#include "output.h"
#include "JETSCAPE_output_to_HDF.h"

using namespace std;

int main(int argc, char ** argv)
{
	if ( argc < 2 )
	{
		cerr << "Usage: ./interpolate_hydro [files to interpolate]" << endl;
		cerr << "E.g.:  ./interpolate_hydro /path/to/results/sveprofile_{0..582}_ev0.dat" << endl;
		return (8);
	}

	// set grid for interpolation
	//cout << endl << " - Generating grid for interpolation" << endl;
	const double xmin = -10.0, ymin = -10.0;
	const double dx = 1.0, dy = 1.0;
	const int xGridSize = 21, yGridSize = 21;
	vector<double> xGrid(xGridSize), yGrid(yGridSize);
	generate(xGrid.begin(), xGrid.end(), [n = 0, &xmin, &dx] () mutable { return xmin + dx * n++; });
	generate(yGrid.begin(), yGrid.end(), [n = 0, &ymin, &dy] () mutable { return ymin + dy * n++; });

	// set up HDF5 stuff
	const H5std_string FILE_NAME("output.h5");
	const double DX = dx, DY = dy, Tau0 = 0.048, tauFS = 1.2, dTau = 0.072;
	const int OutputViscousFlag = 1, XH = 53, XL = -53, YH = 53, YL = -53;

	try
    {
		Exception::dontPrint();
	
		H5File file(FILE_NAME, H5F_ACC_TRUNC);
		Group groupEvent(file.createGroup("/Event"));
		output_attributes( groupEvent, DX, DY, OutputViscousFlag, Tau0, TauFS,
							XH, XL, YH, YL, dTau );

		// eventually loop over files and read in one at a time
		{
			// set filename and load data
			string filename = argv[1];
			
			// outputGrid contains data from filename, interpolated to grid
			// defined by xGrid (x) yGrid
			vector<vector<double> > outputGrid;
			interpolate_hydro_driver( filename, outputGrid, xGrid, yGrid );		

			// send outputGrid to HDF file
			output_dataset( file, outputGrid );
		}

	}

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
