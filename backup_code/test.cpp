#include <algorithm>
#include <iostream>
#include <vector>

using namespace std;

int main()
{
	double xmin = -1.0, dx = 0.2;
	vector<double> xGrid(11);
	generate(xGrid.begin(), xGrid.end(), [n = 0, &xmin, &dx] () mutable { return xmin + dx * n++; });

	for ( const auto & point : xGrid )
		cout << point << endl;

	return 0;
}
