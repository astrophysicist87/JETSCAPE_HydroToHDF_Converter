#ifndef INTERPOLATE_HYDRO_DELAUNAY_H
#define INTERPOLATE_HYDRO_DELAUNAY_H

#include <algorithm>
#include <cmath>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "delaunay/table_delaunay.h"

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

class Delaunay2D
{
	private:
		vector<vector<double> > vertices, faces;
		vector<vector<int> > face_indices;
		vector<vector<double> > value_fields;
		ostream & out;
		ostream & err;

		inline double sign( const vector<double> & p1, const vector<double> & p2, const vector<double> & p3 )
		{
		    return (p1[0] - p3[0]) * (p2[1] - p3[1]) - (p2[0] - p3[0]) * (p1[1] - p3[1]);
		}
		
		bool pointInTriangle ( const vector<double> & pt, const vector<double> & v1,
		                       const vector<double> & v2, const vector<double> & v3 )
		{
		    double d1 = sign(pt, v1, v2);
		    double d2 = sign(pt, v2, v3);
		    double d3 = sign(pt, v3, v1);
		
		    bool has_neg = (d1 < 0.0) || (d2 < 0.0) || (d3 < 0.0);
		    bool has_pos = (d1 > 0.0) || (d2 > 0.0) || (d3 > 0.0);
		
		    return !(has_neg && has_pos);
		}
		
		// this function evaluates the plane passing through v1, v2, v3 at the point p
		double basic_interp( const vector<double> & p, const vector<double> & v1,
		                     const vector<double> & v2, const vector<double> & v3 )
		{
			double det_x = (v2[1]-v1[1])*(v3[2]-v1[2]) - (v3[1]-v1[1])*(v2[2]-v1[2]);
			double det_y = (v2[2]-v1[2])*(v3[0]-v1[0]) - (v3[2]-v1[2])*(v2[0]-v1[0]);
			double det_z = (v2[0]-v1[0])*(v3[1]-v1[1]) - (v3[0]-v1[0])*(v2[1]-v1[1]);
			return ( v1[2] - ( det_x*(p[0] - v1[0]) + det_y*(p[1] - v1[1]) ) / (det_z+1e-100) );
		}

		void initialize_all( const vector<vector<double> > & vertices_in,
                             const vector<vector<double> > & value_fields_in )
		{
			vertices = vertices_in;
			value_fields = value_fields_in;

			// generate grid itself
			generate_delaunay_2d( vertices, faces );

			// associate vertices to the faces that contain them
			vector<vector<int> > face_indices( vertices.size() );
			for ( int iFace = 0 ; iFace < faces.size(); iFace++ )
			for ( int iVertex = 0 ; iVertex < 3; iVertex++ )
				face_indices[ faces[iFace][iVertex] ].push_back( iFace );

			return;
		}

	public:
		Delaunay2D( const vector<vector<double> > & vertices_in,
					const vector<vector<double> > & value_fields_in,
					ostream & out_stream = std::cout,
					ostream & err_stream = std::cerr )
					:
					out(out_stream),
					err(err_stream)
					{ initialize_all( vertices_in, value_fields_in ); };


		void interpolate(vector<vector<double> > & points, vector<double> & outPoint)
		{
			double x0 = outPoint[0];
			double y0 = outPoint[1];

			// find closest vertex to chosen point
			int iClosestVertex = 0;
			double closestDistance2 = d2( vertices[0][0], vertices[0][1], x0, y0 );
			for ( int iVertex = 0; iVertex < vertices.size(); iVertex++ )
			{
				double distance2 = d2( vertices[iVertex][0], vertices[iVertex][1], x0, y0 );
				if ( distance2 < closestDistance2 )
				{
					iClosestVertex = iVertex;
					closestDistance2 = distance2;
				}
			}

			// if chosen point *is* the closest vertex, just return that function value
			if ( sqrt(closestDistance2) < EPS )
			{
				//cout << "Result = " << values[iClosestVertex] << endl;
				outPoint[2] = value_fields[iClosestVertex][0];
				outPoint[3] = value_fields[iClosestVertex][1];
				outPoint[4] = value_fields[iClosestVertex][2];
				outPoint[5] = value_fields[iClosestVertex][3];
			}
			else
			{
				// otherwise, point must be in face containing closest vertex or outside
				// convex hull entirely
				bool no_solution_found_yet = true;
				vector<double> point = { x0, y0 };
				for ( const auto & face_index : face_indices[ iClosestVertex ] )
				{
					// look at next face
					const auto & face = faces[ face_index ];
					vector<double> v0 = vertices[face[0]];
					vector<double> v1 = vertices[face[1]];
					vector<double> v2 = vertices[face[2]];
					/*cout << "Checking face #" << face_index << ":" << endl;
					cout << " --> v0: " << v0[0] << "   " << v0[1] << endl;
					cout << " --> v1: " << v1[0] << "   " << v1[1] << endl;
					cout << " --> v2: " << v2[0] << "   " << v2[1] << endl << endl;*/
					if ( pointInTriangle( point, v0, v1, v2 ) )
					{
						/*v0.push_back( values[ face[0] ] );
						v1.push_back( values[ face[1] ] );
						v2.push_back( values[ face[2] ] );
		
						cout << "Check values: " << v0[2] << "   " << v1[2] << "   " << v2[2] << endl;
		
						cout << "Result = " << basic_interp( point, v0, v1, v2 ) << endl;*/
						v0.resize(3);
						v1.resize(3);
						v2.resize(3);

						for (int iField = 0; iField < 4; iField++)
						{
							v0[2] = value_fields[iField][ face[0] ];
							v1[2] = value_fields[iField][ face[1] ];
							v2[2] = value_fields[iField][ face[2] ];

							outPoint[iField+2] = basic_interp( point, v0, v1, v2 );
						}
		
						no_solution_found_yet = false;
						break;
					}
				}
				if ( no_solution_found_yet )
				{
					//cout << "Result is out of bounds!" << endl;
					outPoint[2] = 0.0;
					outPoint[3] = 0.0;
					outPoint[4] = 0.0;
					outPoint[5] = 0.0;
				}
			}

			return;
		}
		
};

#endif
