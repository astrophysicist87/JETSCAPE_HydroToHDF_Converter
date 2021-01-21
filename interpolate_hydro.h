#ifndef INTERPOLATE_HYDRO_H
#define INTERPOLATE_HYDRO_H

#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

constexpr double EPS = 1e-10;
constexpr double INF = 1e10;

void read_in_data(vector<vector<double> > & points, string filename, int nHeaderLines = 0)
{
	points.clear();

	ifstream infile(filename.c_str());

	if (infile.is_open())
	{
		string line;
		int count = 0;
		while ( getline (infile, line) )
		{
			if ( count++ < nHeaderLines ) continue;

			istringstream iss(line);
			vector<double> point(6);
			for (int iCol = 0; iCol < 6; iCol++)
				iss >> point[iCol];

			// to hold distances
			point.push_back( 0.0 );
			
			points.push_back( point );
		}
	}

	infile.close();
	return;
}


bool sortByDistance( const vector<double> & v1, const vector<double> & v2 ) {
  return ( v1[6] < v2[6] );
}


void interpolate(vector<vector<double> > & points, vector<double> & outPoint)
{
	double x = outPoint[0];
	double y = outPoint[1];

	//the first col is x axis, the second col is y axis
	for ( auto & point : points )
	{
		double dx = point[0]-x, dy = point[1]-y;
		//point[6] = sqrt(dx*dx+dy*dy);	//distance
		point[6] = dx*dx+dy*dy;	//distance^2
		if (point[2] < EPS || point[3] < 0.15)
		{
			point[6] = INF; //if energy = 0, set distance to be large enough to eliminate the influence
			continue;
		}
	}
	
	// partial sort only smallest 8 distances
	sort(points.begin(), points.end(), sortByDistance);
	//std::nth_element( points.begin(), points.begin() + 7, points.end(), sortByDistance );
	
	int num = 8;
	double totalrate   = 0;
	double totaltemp   = 0;
	double totalvinx   = 0;
	double totalviny   = 0;
	double totalenergy = 0;
	
	for( int i = 0; i < num; i++)
	{
		const auto point = points[i];

		// if the distance is 0, which means they are the same point,
		// copy all the info instead of doing interpolation
		if (point[6] < EPS)
		{
			totaltemp   = point[3];
			totalvinx   = point[4];
			totalviny   = point[5];
			totalenergy = point[2];
			break;
		}
		else
		{
			// f += pow(1.0 / point[3], r);
			double rate  = 1.0 / point[6];
			totalrate   += rate;
			totalenergy += point[2] * rate;
			totaltemp   += point[3] * rate;
			totalvinx   += point[4] * rate;
			totalviny   += point[5] * rate;
		}
	}
	
	totaltemp   /= totalrate;
	totalvinx   /= totalrate;
	totalviny   /= totalrate;
	totalenergy /= totalrate;
	
	/*std::cout <<"Interpolation temp " <<totaltemp << '\n';
	std::cout <<"Interpolation velocity in x  " <<totalvinx << '\n';
	std::cout <<"Interpolation velocity in y  " <<totalviny << '\n';
	std::cout <<"Interpolation energydensity " <<totalenergy << '\n';*/

	outPoint[2] = totalenergy;
	outPoint[3] = totaltemp;
	outPoint[4] = totalvinx;
	outPoint[5] = totalviny;
	
	return;
}

#endif
