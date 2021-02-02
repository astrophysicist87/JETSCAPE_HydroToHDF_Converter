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
	const int	 NY = (v[0]).size();
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
		error.printError();
		return;
    }

    catch(DataSetIException error)
    {
		error.printError();
		return;
    }

    catch(DataSpaceIException error)
    {
		error.printError();
		return;
    }
}


void output_to_HDF_for_JETSCAPE( const vector<vector<double> > & v, string outfilename )
{
	const H5std_string	FILE_NAME(outfilename.c_str());
	const H5std_string	DATASET_NAME("hydro");
	const int	 NX = v.size();
	const int	 NY = (v[0]).size();
	const int	 RANK = 2;

	double data[NX][NY];
	for (int ix = 0; ix < NX; ix++)
	for (int iy = 0; iy < NY; iy++)
		data[ix][iy] = v[ix][iy];

    try
    {
		Exception::dontPrint();
	
		//H5File file(FILE_NAME, H5F_ACC_TRUNC);
		//H5File* file = new H5File(FILE_NAME, H5F_ACC_TRUNC);
	
    	//Group group = file.createGroup( "/" );
    	//Group eventGroup = file.createGroup( "/EVENT" );
    	//Group* group = new Group( file->createGroup( "/" ) );
    	//Group* eventGroup = new Group( file.createGroup( "/EVENT" ) );

		//double DX = 0.1;
		//DataSpace dspace(H5T_IEEE_F64LE);
		//Attribute* att = new Attribute( group->createAttribute("DX", PredType::NATIVE_DOUBLE, dspace ) );
		//att->write(PredType::NATIVE_DOUBLE, &DX);
		/*ATTRIBUTE "DX" {
         DATATYPE  H5T_IEEE_F64LE
         DATASPACE  SIMPLE { ( 1 ) / ( 1 ) }
         DATA {
         (0): 0.288
         }*/
		/*hsize_t dims[2];
		dims[0] = NX;
		dims[1] = NY;
		DataSpace dataspace(RANK, dims);
	
		DataSet dataset = file.createDataSet(DATASET_NAME, PredType::NATIVE_DOUBLE, dataspace);
			
		dataset.write(data, PredType::NATIVE_DOUBLE);*/



		//Create a new file using default properties
		H5File file("NewH5.h5", H5F_ACC_TRUNC);

		//Create PLATFORM_t and SONAR_t groups in the file
		Group groupPlatform(file.createGroup("/PLATFORM_t"));
	
		double value=42;
		DataSpace dspace(H5S_SCALAR);
		Attribute att = groupPlatform.createAttribute("some_attribute",PredType::NATIVE_DOUBLE,dspace);
		att.write(PredType::NATIVE_DOUBLE,&value);
    }

    catch(FileIException error)
    {
		error.printError();
		return;
    }

    catch(DataSetIException error)
    {
		error.printError();
		return;
    }

    catch(DataSpaceIException error)
    {
		error.printError();
		return;
    }
}


#endif
