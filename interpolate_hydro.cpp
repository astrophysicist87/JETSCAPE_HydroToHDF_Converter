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
	// make sure at least a filename was passed
	//assert( argc > 1 );

	if ( argc < 2 )
	{
		cout << "// No command line argument passed --> running in test mode" << endl;
		vector<vector<double> > points;
		read_in_data(points, "backup_code/data8_ev1.dat", 1);
		vector<double> testPoint (6);
		testPoint[0] = 0.504858;
		testPoint[1] = 6.58063;
		interpolate( points, testPoint );
		cout << "Results: ";
		for ( const auto & c : testPoint ) cout << c << "   ";
		cout << endl;
		return 0;
	}

	cout << "////////////////////////////////////////////////////////////////////" << endl;
	cout << "// Conversion and interpolation of v-USPhydro files to HDF format //" << endl;
	cout << "////////////////////////////////////////////////////////////////////" << endl;

	// set filename and load data
	string filename = argv[1];

	cout << endl << " - Reading in data from " << filename << endl;
	vector<vector<double> > points;
	read_in_data(points, filename, 1);

	// set grid for interpolation
	cout << endl << " - Generating grid for interpolation" << endl;
	const double xmin = -10.0, ymin = -10.0;
	const double dx = 0.1, dy = 0.1;
	const int xGridSize = 201, yGridSize = 201;
	vector<double> xGrid(xGridSize), yGrid(yGridSize);
	generate(xGrid.begin(), xGrid.end(), [n = 0, &xmin, &dx] () mutable { return xmin + dx * n++; });
	generate(yGrid.begin(), yGrid.end(), [n = 0, &ymin, &dy] () mutable { return ymin + dy * n++; });

	// set up output grid and do the interpolation
	cout << endl << " - Performing interpolation" << endl;
	vector<vector<double> > outputGrid( xGridSize*yGridSize, vector<double> ( 6 ) );
	int idx = 0;
	for ( int ix = 0; ix < xGridSize; ix++ )
	for ( int iy = 0; iy < yGridSize; iy++ )
	{
		outputGrid[idx][0] = xGrid[ix];
		outputGrid[idx][1] = yGrid[iy];
		//interpolate( points, outputGrid[idx] );
		idx++;
	}

	// output to both dat and HDF files...
	cout << endl << " - Outputting to *.dat file" << endl;
	output_to_dat( outputGrid, "output.dat" );

	cout << endl << " - Outputting to *.hdf file" << endl;
	//output_to_HDF( outputGrid, "output.h5" );
	output_to_HDF_for_JETSCAPE( outputGrid, "output.h5" );

	cout << endl << " - Finished everything!" << endl;
	return 0;
}
