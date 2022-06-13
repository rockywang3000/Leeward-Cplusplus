#include <iostream>
#include "set.cpp"

using namespace std;

class aMatrix
{

public:
	aMatrix(long dimension);  //constructor
	
	//Don't return aMatrix object in their own memeber functions
	//Use reference to pass aMatrix object parameters in their own member functions
	//Declare aMatrix object in main function and release memory there
	virtual ~aMatrix(); 

	//more like a constructor without allocating memory
	void initialize();

	//conversion from aMatrix to the index of the 1-dimension array
	long index(long row, long column);
	void setValue(long row,long column, double value);
	double getValue(long row,long column);
	void symmetry();

	// useful for debugging
	void print(char* info);

	//operate the elements
	void zero();
	void minusOne();

	//the most important function
	void guess(aMatrix &result,aMatrix &prevGuess,long dimension,long k,
		char ** matrixArray);

	//arithmetic calculations
	void plus(aMatrix &result,aMatrix &a);
	void constantMinus(aMatrix &result,double constant);
	void multiply(aMatrix &result, aMatrix &coeff);
	void multiply(aMatrix &result, double constant);

	// for converging faster uses
	void vibrate(aMatrix &first, aMatrix &second, aMatrix &third);
	void alphaUpdate(aMatrix &result);

	void copy(aMatrix &result); //copy

	//average two matrixes
	void update(aMatrix &temp);
	void negativeOne();

	bool IsZero();

	// a=1/b except (-1 elements)
	void inverse(aMatrix &result);

	//judge if two matrixes are same enough
	void SameUpdate(aMatrix &first, aMatrix &compare,double delta);


	//let the resule make sense
	// e.g. if the inversed result R is between 1 ohm and some positive value
	// then G matrix should between 0 and 1
	void rationalize();

	//anxiliary functions for statistics use to tell if two guesses are same enough
	//max returns the maximum difference percentage,average returns the average one
	void squareError(aMatrix &compare, double &average,double &max);

	//Find the neighbor set for node k
	Set<long> nearest(long k);

private:

	long n; //dimension
	double * myMatrix; // n*n aMatrix
};


