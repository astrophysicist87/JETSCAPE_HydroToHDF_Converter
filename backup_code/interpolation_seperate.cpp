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
#include <chrono>

using namespace std;
using namespace std::chrono;

void readFromFile(vector<vector<double> > *points) {
  std::ifstream file;
  file.open("data8_ev1.dat");
  double data;
  vector<double> one_row;
	vector<double> empty;
	std::vector<std::vector<double> > V;

  int index = 1;
  file >> data;
	file >> data;
  assert (!file.fail( ));

  while (!file.eof( ))  {
    if(index%7 != 0) {
			one_row.push_back(data);
    }else if(index%7 == 0) {
			one_row.push_back(0);
			V.push_back(one_row);
			one_row = empty;
    }
		file >> data;
    index++;
  }

	*points = V;

  file.close();
}

bool sortcol( const vector<double>& v1, const vector<double>& v2 ) {
  return v1[6] < v2[6];
}

double interpolation(vector<vector<double> > points, double x, double y) {
  //the first col is x axis, the second col is y axis

  for(int k = 0; k < 50205;k++) {
cout << "k = " << k << endl;
    (points)[k][6] = sqrt(pow(points[k][0]-x,2)+pow(points[k][1]-y,2));//distance
    if((points)[k][2] == 0 || points[k][3] < 0.15) {
      (points)[k][6] = 101; //if energy = 0, set distance to be large enough to eliminate the influence
      continue;
    }
  }

  sort(points.begin(),points.end(),sortcol);

  int num = 8;
  int i;

	double totalrate = 0;
  double totaltemp = 0;
  double totalvinx = 0;
  double totalviny = 0;
	double totalenergy = 0;


  for( i = 0; i < num; i++) {
    //if the distance is 0, which means they are the same point, copy all the info instead of doing interpolation
    if (points[i][6] == 0) {
      totaltemp = points[i][3];
			totalvinx = points[i][4];
			totalviny = points[i][5];
			totalenergy = points[i][2];
    }else{
      // f += pow(1.0 / points[i][3], r);
			double rate = 1.0 / points[i][6];
      totalrate += rate;
			totaltemp += points[i][3] * rate;
			totalvinx += points[i][4] * rate;
			totalviny += points[i][5] * rate;
			totalenergy += points[i][2] * rate;
    }
  }

	totaltemp = totaltemp/totalrate;
	totalvinx = totalvinx/totalrate;
	totalviny = totalviny/totalrate;
	totalenergy = totalenergy/totalrate;

	std::cout <<"Interpolation temp " <<totaltemp << '\n';
	std::cout <<"Interpolation velocity in x  " <<totalvinx << '\n';
	std::cout <<"Interpolation velocity in y  " <<totalviny << '\n';
	std::cout <<"Interpolation energydensity " <<totalenergy << '\n';

  return 0;
}

int main()
{
  vector<vector<double> > points(50205, vector<double> (7)); // 50205 is the number of rows
  readFromFile(&points);
	interpolation(points,0.504858,6.58063);//interpolation(points,x coordinate, y coordinate)


	return 0;
}
