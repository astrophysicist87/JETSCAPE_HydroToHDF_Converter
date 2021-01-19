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

using namespace std;

int main(int argc, char ** argv)
{
	// make sure at least a filename was passed
	assert( argc > 1 );

	// set filename and load data
	string filename = argv[1];
	read_in_data(points, filename, 1);

	// set grid for interpolation
	const double xmin = -10.0, ymin = -10.0;
	const double dx = 0.1, dy = 0.1;
	const int xGridSize = 201, yGridSize = 201;
	vector<double> xGrid(xGridSize), yGrid(yGridSize);
	generate(xGrid.begin(), xGrid.end(), [n = 0, &xmin, &dx] () mutable { return xmin + dx * n++; });
	generate(yGrid.begin(), yGrid.end(), [n = 0, &ymin, &dy] () mutable { return ymin + dy * n++; });

	// set up output grid and do the interpolation
	vector<vector<double> > outputGrid( xGridSize*yGridSize, vector<double> ( 6 ) );
	int idx = 0;
	for ( int ix = 0; ix < xGridSize; ix++ )
	for ( int iy = 0; iy < yGridSize; iy++ )
	{
		outputGrid[idx][0] = xGrid[ix];
		outputGrid[idx][1] = yGrid[iy];
		interpolate( points, outputGrid[idx] );
	}

	// output to both dat and HDF files...
	output_to_dat( outputGrid, "output.dat" );
	output_to_HDF( outputGrid, "output.h5" );

	return 0;
}
