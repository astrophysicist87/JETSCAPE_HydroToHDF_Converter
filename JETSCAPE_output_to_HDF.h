#ifndef JETSCAPE_OUTPUT_TO_HDF_H
#define JETSCAPE_OUTPUT_TO_HDF_H

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

void output_double_attribute(Group & groupEvent, double value, string name)
{
	hsize_t dims[1]; dims[0] = 1;
	DataSpace dspace(1, dims);
	Attribute att = groupEvent.createAttribute(name.c_str(), PredType::NATIVE_DOUBLE, dspace );
	att.write(PredType::NATIVE_DOUBLE, &value);

	return;
}

void output_int_attribute(Group & groupEvent, int value, string name)
{
	hsize_t dims[1]; dims[0] = 1;
	DataSpace dspace(1, dims);
	Attribute att = groupEvent.createAttribute(name.c_str(), PredType::NATIVE_INT, dspace );
	att.write(PredType::NATIVE_INT, &value);

	return;
}

void output_attributes(
		Group & groupEvent, double DX, double DY,
		int OutputViscousFlag, double Tau0, double TauFS,
		int XH, int XL, int YH, int YL, double dTau )
{
	//double DX = 0.1;
	//const int RANK = 1;
	//hsize_t dims[RANK];
	//dims[0] = 1;
	//DataSpace dspace(RANK, dims);
	//Attribute att = groupEvent.createAttribute("DX", PredType::NATIVE_DOUBLE, dspace );
	//att.write(PredType::NATIVE_DOUBLE, &DX);

	output_double_attribute( groupEvent, DX, "DX" );
	output_double_attribute( groupEvent, DY, "DY" );
	output_int_attribute(    groupEvent, OutputViscousFlag, "OutputViscousFlag" );
	output_double_attribute( groupEvent, Tau0, "Tau0" );
	output_double_attribute( groupEvent, TauFS, "TauFS" );
	output_int_attribute(    groupEvent, XH, "XH" );
	output_int_attribute(    groupEvent, XL, "XL" );
	output_int_attribute(    groupEvent, YH, "YH" );
	output_int_attribute(    groupEvent, YL, "YL" );
	output_double_attribute( groupEvent, dTau, "dTau" );

	return;
}

void output_dataset(H5File & file, vector<vector<double> > & v)
{
	const int NX = v.size();
	const int NY = (v[0]).size();

	double data[NX][NY];
	for (int ix = 0; ix < NX; ix++)
	for (int iy = 0; iy < NY; iy++)
		data[ix][iy] = v[ix][iy];

	Group groupFrame(file.createGroup("/Event/Frame_0000"));
	hsize_t dims[2];
	dims[0] = NX;
	dims[1] = NY;
	DataSpace dataspace(2, dims);

	DataSet dataset = groupFrame.createDataSet("/Event/Frame_0000/hydroCheck",
												PredType::NATIVE_DOUBLE, dataspace);
		
	dataset.write(data, PredType::NATIVE_DOUBLE);
	return;
}

/*void output_to_HDF_for_JETSCAPE( const vector<vector<double> > & v, string outfilename )
{
	const H5std_string	FILE_NAME(outfilename.c_str());
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

		Group groupEvent(file.createGroup("/Event"));

		output_attributes( groupEvent );

		{	
			Group groupFrame(file.createGroup("/Event/Frame_0000"));
			hsize_t loc_dims[2];
			loc_dims[0] = NX;
			loc_dims[1] = NY;
			DataSpace dataspace(2, loc_dims);
		
			DataSet dataset = groupFrame.createDataSet("/Event/Frame_0000/hydro",
														PredType::NATIVE_DOUBLE, dataspace);
				
			dataset.write(data, PredType::NATIVE_DOUBLE);
		}
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
}*/


#endif
