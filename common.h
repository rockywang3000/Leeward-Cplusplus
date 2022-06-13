#define numNonzero 10000 //customized for applications
struct compactSparse
{
	long n; //dimension of the matrix, not the size of each vector!!
	long number;//how many nonzero elements stored
	int row[numNonzero];
	int column[numNonzero];
	double value[numNonzero];
};