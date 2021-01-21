#include <iostream>
#include <random>
#include <chrono>
#include <vector>

#include "table_delaunay.h"

using namespace std;

// use a simple smooth function
inline double f(double x, double y) { return ( exp(-x*x - y*y) ); }
inline double d2( double x, double y, double x0, double y0 )
{
	return ( ( x - x0 )*( x - x0 ) + ( y - y0 )*( y - y0 ) );
}
inline double sign( double x0, double y0, double x1, double y1, double x2, double y2 )
{
	return (x0 - x2) * (y1 - y2) - (x1 - x2) * (y0 - y2);
}
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

void set_random_vertices( const int n, vector<vector<double> > & v )
{
	//unsigned seed = chrono::system_clock::now().time_since_epoch().count();
	const int seed = 0;

	default_random_engine generator (seed);
	normal_distribution<double> distribution(0.0, 1.0);
	
	for ( int i = 0; i < n; i++ )
		v.push_back( { distribution(generator), distribution(generator) } );

	return;
}

int main(int argc, char *argv[])
{
	vector<vector<double> > vertices, faces;

	// generate vertices
	set_random_vertices( 100, vertices );

	//for ( const auto & v : vertices )
	//	cout << v[0] << "   " << v[1] << endl;

	// generate corresponding Delaunay triangulation
	generate_delaunay_2d( vertices, faces );

	/*cout << "Found the following triangulation:" << endl;
	for ( int iFace = 0 ; iFace < faces.size(); iFace++ )
	{
		for ( int iVertex = 0 ; iVertex < 3; iVertex++ )
			cout << faces[iFace][iVertex] << "   ";
		cout << endl;
	}*/

	// associate vertices to the faces that contain them
	vector<vector<int> > face_indices( vertices.size() );
	for ( int iFace = 0 ; iFace < faces.size(); iFace++ )
	for ( int iVertex = 0 ; iVertex < 3; iVertex++ )
		face_indices[ faces[iFace][iVertex] ].push_back( iFace );

	// set function values at vertices
	vector<double> values;
	for ( const auto & vertex : vertices )
		values.push_back( f( vertex[0], vertex[1] ) );

	// pick a point at which to perform interpolation
	const double x0 = 0.8, y0 = 1.1;

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

	cout << "Closest distance = " << sqrt(closestDistance2) << endl;

	// if chosen point *is* the closest vertex, just return that function value
	if ( sqrt(closestDistance2) < 1e-10 )
	{
		cout << "Result = " << values[iClosestVertex] << endl;
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
			cout << "Checking face #" << face_index << ":" << endl;
			cout << " --> v0: " << v0[0] << "   " << v0[1] << endl;
			cout << " --> v1: " << v1[0] << "   " << v1[1] << endl;
			cout << " --> v2: " << v2[0] << "   " << v2[1] << endl << endl;
			if ( pointInTriangle( point, v0, v1, v2 ) )
			{
				v0.push_back( values[ face[0] ] );
				v1.push_back( values[ face[1] ] );
				v2.push_back( values[ face[2] ] );

				cout << "Check values: " << v0[2] << "   " << v1[2] << "   " << v2[2] << endl;

				cout << "Result = " << basic_interp( point, v0, v1, v2 ) << endl;

				no_solution_found_yet = false;
				break;
			}
		}
		if ( no_solution_found_yet )
			cout << "Result is out of bounds!" << endl;
	}

	cout << "Exact = " << f( x0, y0 ) << endl;

	return 0;
}
