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
		cerr << "Usage: ./interpolate_hydro [files to interpolate]
		cerr << "E.g.:  ./interpolate_hydro /path/to/results/sveprofile_{0..582}_ev0.dat
		return (8);
	}

	// set grid for interpolation
	//cout << endl << " - Generating grid for interpolation" << endl;
	const double xmin = -10.0, ymin = -10.0;
	const double dx = 0.1, dy = 0.1;
	const int xGridSize = 201, yGridSize = 201;
	vector<double> xGrid(xGridSize), yGrid(yGridSize);
	generate(xGrid.begin(), xGrid.end(), [n = 0, &xmin, &dx] () mutable { return xmin + dx * n++; });
	generate(yGrid.begin(), yGrid.end(), [n = 0, &ymin, &dy] () mutable { return ymin + dy * n++; });

	// set up HDF5 stuff
	const H5std_string	FILE_NAME(filename.c_str());
	const int	 NX = xGridSize*yGridSize;
	const int	 NY = 6;

	try
    {
		Exception::dontPrint();
	
		//H5File file(FILE_NAME, H5F_ACC_TRUNC);
		H5File file("output.h5", H5F_ACC_TRUNC);
		Group groupEvent(file.createGroup("/Event"));
		output_attributes( groupEvent );	// add attributes later

		// eventually loop over files and read in one at a time
		{
			// set filename and load data
			string filename = argv[1];
			
			vector<vector<double> > outputGrid;
			interpolate_hydro_driver( filename, outputGrid, xGrid, yGrid );		

			double data[NX][NY];
			for (int ix = 0; ix < NX; ix++)
			for (int iy = 0; iy < NY; iy++)
				data[ix][iy] = outputGrid[ix][iy];

			Group groupFrame(file.createGroup("/Event/Frame_0000"));
			hsize_t dims[2];
			dims[0] = NX;
			dims[1] = NY;
			DataSpace dataspace(2, loc_dims);
		
			DataSet dataset = groupFrame.createDataSet("/Event/Frame_0000/hydroCheck",
														PredType::NATIVE_DOUBLE, dataspace);
				
			dataset.write(data, PredType::NATIVE_DOUBLE);
		}

	}

	return 0;
}
