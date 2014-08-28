#include "MatlabFile.h"

#include <iostream>
#include <string>

using namespace std;

/*
 Example usage of MatlabFile
 */
int main(int argc, char** argv)
{

	// Need these
	float2* a;
	float2* b;
	unsigned int x = 0;
	unsigned int y = 0;

	// Read
	MatlabFile mat_read(READ);
	mat_read.open("example_input.mat");
	mat_read.readCompoundArray("a_1d_C", a, x, y);
	mat_read.close();

	// a, b are now allocated and populated
	for (int i = 0; i < y; ++i)
	{
		a[i].x /= 10;
		a[i].y /= 10;
	}

	// Write
	MatlabFile mat_write(WRITE);
	mat_write.open("example_output.mat");
	mat_write.writeCompoundArray("b_1d_C", a, x, y);
	mat_write.close();

	// free
	free(a);
	free(b);

	return EXIT_SUCCESS;
}
