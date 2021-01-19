#ifndef OUTPUT_H
#define OUTPUT_H

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "H5Cpp.h"

#ifndef H5_NO_NAMESPACE
    using namespace H5;
#endif

void output_to_dat( const vector<vector<double> > & v, string outfilename )
{
	ofstream outfile(outfilename.c_str());

	for ( const auto & row : v )
	{
		outfile << setw(12) << setprecision(8);
		for ( const auto & column : row )
			outfile << column << "   ";
		outfile << endl;
	}

	outfile.close();
	return;
}

void output_to_HDF( const vector<vector<double> > & v, string outfilename )
{
	const H5std_string	FILE_NAME(outfilename.c_str());
	const H5std_string	DATASET_NAME("hydro");
	const int	 NX = v.size();
	const int	 NY = (v.begin()).size();
	const int	 RANK = 2;

	double data[NX][NY];
	for (int ix = 0; ix < NX; ix++)
	for (int iy = 0; iy < NY; iy++)
		data[ix][iy] = v[ix][iy];

    try
    {
		Exception::dontPrint();
	
		H5File file(FILE_NAME, H5F_ACC_TRUNC);
	
		hsize_t dims[2];
		dims[0] = NX;
		dims[1] = NY;
		DataSpace dataspace(RANK, dims);
	
		DataSet dataset = file.createDataSet(DATASET_NAME, PredType::NATIVE_DOUBLE, dataspace);
			
		dataset.write(data, PredType::NATIVE_DOUBLE);
    }

    catch(FileIException error)
    {
		error.printErrorStack();
		return;
    }

    catch(DataSetIException error)
    {
		error.printErrorStack();
		return;
    }

    catch(DataSpaceIException error)
    {
		error.printErrorStack();
		return;
    }
}


#endif
