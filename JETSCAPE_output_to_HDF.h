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

//==============================================================================
//==============================================================================
string get_zero_padded_int( int i, int width )
{
	std::ostringstream ss;
    ss << std::setw( width ) << std::setfill( '0' ) << i;
    return ss.str();
}

//==============================================================================
//==============================================================================
void output_double_attribute(Group & group, double value, string name)
{
	hsize_t dims[1]; dims[0] = 1;
	DataSpace dspace(1, dims);
	Attribute att = group.createAttribute(name.c_str(), PredType::NATIVE_DOUBLE, dspace );
	att.write(PredType::NATIVE_DOUBLE, &value);

	return;
}

//==============================================================================
//==============================================================================
void output_int_attribute(Group & group, int value, string name)
{
	hsize_t dims[1]; dims[0] = 1;
	DataSpace dspace(1, dims);
	Attribute att = group.createAttribute(name.c_str(), PredType::NATIVE_INT, dspace );
	att.write(PredType::NATIVE_INT, &value);

	return;
}

//==============================================================================
//==============================================================================
void output_attributes(
		Group & groupEvent, double DX, double DY,
		int OutputViscousFlag, double Tau0, double TauFS,
		int XH, int XL, int YH, int YL, double dTau )
{
	output_double_attribute( groupEvent, DX,                "DX"                );
	output_double_attribute( groupEvent, DY,                "DY"                );
	output_int_attribute(    groupEvent, OutputViscousFlag, "OutputViscousFlag" );
	output_double_attribute( groupEvent, Tau0,              "Tau0"              );
	output_double_attribute( groupEvent, TauFS,             "TauFS"             );
	output_int_attribute(    groupEvent, XH,                "XH"                );
	output_int_attribute(    groupEvent, XL,                "XL"                );
	output_int_attribute(    groupEvent, YH,                "YH"                );
	output_int_attribute(    groupEvent, YL,                "YL"                );
	output_double_attribute( groupEvent, dTau,              "dTau"              );

	return;
}

//==============================================================================
//==============================================================================
void output_dataset( H5File & file, string FRAME_NAME, const double time, 
					 vector<vector<double> > & v, const int NX, const int NY, const int NZ)
{
	// NX, NY are grid sizes in x and y directions, respectively
	// NZ is the number of quantities being interpolated, with x and y coordinates
	// (i.e., for x, y, T, e, ux, and uy, NZ == 6)
	double data[NZ-2][NX][NY];
	for (int ix = 0; ix < NX; ix++)
	for (int iy = 0; iy < NY; iy++)
	for (int iz = 2; iz < NZ; iz++)	// skip x and y coordinates
		data[iz-2][ix][iy] = v[ix*NY+iy][iz];

	Group groupFrame(file.createGroup(FRAME_NAME.c_str()));

	// some frames apparently come with a timestamp(?)
	output_double_attribute( groupFrame, time, "Time" );

	// name the different quantities
	vector<string> dataset_names = {"/e", "/Temp", "/ux", "/uy"};
	for (int iDS = 0; iDS < dataset_names.size(); iDS++)
	{
		hsize_t dims[2];
		dims[0] = NX;
		dims[1] = NY;
		DataSpace dataspace(2, dims);
	
		string DATASET_NAME = FRAME_NAME + dataset_names[iDS];
		DataSet dataset = groupFrame.createDataSet( DATASET_NAME.c_str(),
													PredType::NATIVE_DOUBLE, dataspace);
			
		dataset.write(data[iDS], PredType::NATIVE_DOUBLE);
	}
	return;
}

#endif
