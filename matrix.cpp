#include "matrix.h"
#include <iostream>
#include "nrutil.h"
#include <math.h>
#include "common.h"


//#include "sptest.c"
#define TINY 1.0e-20

//import function from sptest.c 
extern "C" {
void sparse13(struct compactSparse* coeff,double *RVector,long k,
			  char ** matrixArray);
}


// test if the linear equations are solved correctly
// test if A * X = B ?
void test(double ** A,double *X,double *B,long n);


/********************************************************
*  uses: constructor
*  precondition: long dimension
*  postcondition: initialize the aMatrix object
********************************************************/
aMatrix::aMatrix(long dimension)//constructor
{
	// use one dimension array to simulate 2-dimension aMatrix
	myMatrix = new double[dimension*dimension];
	n = dimension;

	// suppose the aMatrix elements can't be negative
	for (long row=1; row <= dimension; row++)
	{
		for ( long column=1; column <= dimension; column++ )
		{
			setValue(row,column,-1); //means unknow
		}
	}
}

void
aMatrix::initialize()
{
	// suppose the aMatrix elements can't be negative
	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			setValue(row,column,-1); //means unknow
		}
	}
}

//destructor
aMatrix::~aMatrix ()
{
	if (myMatrix != NULL) 
		delete [] myMatrix;

	myMatrix = NULL;
}


//reset all elements to 0
void 
aMatrix::zero ()
{
	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			setValue(row,column,0);
		}
	}
	return;
}

//reset all elements to -1
void 
aMatrix::minusOne ()
{
	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			setValue(row,column,-1);
		}
	}
	return;
}

/**************************************************
The following three functions provide accesses to elements
with indices
**************************************************/
/*
 
11 12 13 ...
21 22 23 ...
31 32 33 ...
.
.
.

*/
long aMatrix::index(long row, long column)
{
	return (row-1) * n + column-1;
}

void aMatrix::setValue(long row,long column, double value)
{
	myMatrix[(row-1)*n+column-1] = value;
}

double aMatrix::getValue(long row,long column)
{
	return myMatrix[(row-1)*n+column-1];
}


// make the aMatrix symmetrical, except -1 elements
void aMatrix::symmetry()
{
	double temp;

	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			temp = getValue(row,column);
			if(  temp != -1)
				setValue(column,row,temp); 
		}
	}

	return;
}



// make the matrix elements between 0 and 1
// if less then 0, set it to 0.000000001
// if bigger than 1, set it to 1.
void 
aMatrix::rationalize()
{
	double temp;

	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			temp = getValue(row,column);

			if (temp != -1)
			{
				if(temp < 0)
					setValue(row,column,0);		
				else if(temp > 1)
					setValue(row,column,1);
			}
		}
	}

	return;
}


// for set purpose
long mPositionInZ(long m, Set<long>Z)
{
	long i;
	long temp;
	for(i=1;i<=Z.size ();i++)
	{
		temp=Z.setArray [i-1];
		if(temp==m)
			return i;
	}
	return i;
}


/*
input: prevGuess is a G-aMatrix
 Use prevGuess aMatrix,long k and Voltage aMatrix to calculate the nextGuess aMatrix
output: nextGuess is the returned UPDATED G-aMatrix
*/
void aMatrix::guess(aMatrix &nextGuess,aMatrix &prevGuess,long dimension,long k,
					char ** matrixArray)
{
/*The solution order of coeff aMatrix
  [Gk0,Glk,Zpk]
  e.g. N={1,2,3,4,5,6} k=2
  Nk={4,6} tempK ={2}
  Z= {1,3,5}

  [G20,G42,G62,Z12,Z32,Z52]
  ---- ------- ------------
	2    4,6     1,3,5

*/
	compactSparse * CoeffOpt = new compactSparse;
	CoeffOpt->n=dimension;
	CoeffOpt->number =0;

	double *rhsV = dvector(1,n);
	long i;
	Set<long> N,Nk, Nk_,tempK,Z;
	tempK.insert (k);

	//Get N
	for(i=1; i<=dimension;i++)
	{
		N.insert (i);
	}

	Nk= nearest(k);
	Nk.sort();
	Nk_ = N - Nk;
	Nk_.sort ();
	Z= Nk_ - tempK;
	Z.sort ();

	//temporary variables
	int tempRow,tempCol;
	double tempVal;

	//The first group of function:
	tempRow =1;
	tempCol=1;
	tempVal = getValue(k,k);

	CoeffOpt->row[CoeffOpt->number] = tempRow;
	CoeffOpt->column[CoeffOpt->number] = tempCol;
	CoeffOpt->value[CoeffOpt->number] = tempVal;
	CoeffOpt->number ++;

	for(i=1;i<= Nk.size();i++)
	{
		tempRow =1;
		tempCol=i+1;
		tempVal = getValue(k,k)-getValue(Nk.setArray[i-1],k);

		//update coefficient matrix
		CoeffOpt->row[CoeffOpt->number] = tempRow;
		CoeffOpt->column[CoeffOpt->number] = tempCol;
		CoeffOpt->value[CoeffOpt->number] = tempVal;
		CoeffOpt->number ++;
	}

	//set RVector value
	rhsV[1] = 1;

	//------------------------------------------------------
	//The second group of functions
	long j,l;
	Set<long> Nl,m1,m2;
	for(i=1;i<= Nk.size ();i++)
	{
		l = Nk.setArray [i-1];
		Nl= nearest(l);
		Nl.sort ();
		m1= Nl&&Nk;
		m1.sort ();
		m2= Nl&&Nk_ - tempK;
		m2.sort ();

		//Glk,Zmk variables--- we only need their position info.

		tempRow =i+1;
		tempCol=i+1;
		tempVal = getValue(l,k)-getValue(k,k);

		CoeffOpt->row[CoeffOpt->number] = tempRow;
		CoeffOpt->column[CoeffOpt->number] = tempCol;
		CoeffOpt->value[CoeffOpt->number] = tempVal;
		CoeffOpt->number ++;

		for(j=1;j<=m2.size ();j++)
		{
			tempRow =i+1;
			tempCol=1+Nk.size ()+mPositionInZ(m2.setArray[j-1],Z);
			tempVal = -prevGuess.getValue(m2.setArray [j-1],l);

			CoeffOpt->row[CoeffOpt->number] = tempRow;
			CoeffOpt->column[CoeffOpt->number] = tempCol;
			CoeffOpt->value[CoeffOpt->number] = tempVal;
			CoeffOpt->number ++;
		}
	
		//calculate RVector
		double sum1=0,sum2=0;

		for(j=1;j<=m1.size();j++)
		{
			sum1=sum1+prevGuess.getValue (m1.setArray [j-1],l)*
				(getValue(l,k)-getValue(m1.setArray [j-1],k));
		}
		for(j=1;j<=m2.size();j++)
		{
			sum2=sum2+prevGuess.getValue (m2.setArray [j-1],l)*
				getValue(l,k);
		}
 
		rhsV[i+1]=-prevGuess.getValue(l,l)*getValue(l,k)-sum1-sum2;
	}

	//------------------------------------------------------
	//The third group
	Set<long> P;
	P = Nk_-tempK;
	P.sort ();

	for(i=1;i<= P.size ();i++)
	{
		long p = P.setArray [i-1];
		Set<long>Np = nearest(p);
		Np.sort ();
		Set<long>q1= Np&&Nk_-tempK;
		q1.sort ();
		Set<long>q2= Np&&Nk;
		q2.sort ();

		//convenient for indexing
		long startingRow = i+1+Nk.size();

		for(j=1;j<=q1.size ();j++)
		{
			tempRow =startingRow;
			tempCol=1+Nk.size ()+mPositionInZ(q1.setArray[j-1],Z);
			tempVal = -prevGuess.getValue(q1.setArray [j-1],p);

			CoeffOpt->row[CoeffOpt->number] = tempRow;
			CoeffOpt->column[CoeffOpt->number] = tempCol;
			CoeffOpt->value[CoeffOpt->number] = tempVal;
			CoeffOpt->number ++;
		}	

		//Zpk
		double sum1=0,sum2=0;
		for(j=1;j<=q1.size();j++)
		{
			sum1=sum1+prevGuess.getValue (q1.setArray [j-1],p);
		}

		for(j=1;j<=q2.size();j++)
		{
			sum2=sum2+prevGuess.getValue (q2.setArray [j-1],p);
		}
 
		tempRow =startingRow;
		tempCol=1+Nk.size ()+mPositionInZ(p,Z);
		tempVal = prevGuess.getValue(p,p)+sum1+sum2;

		CoeffOpt->row[CoeffOpt->number] = tempRow;
		CoeffOpt->column[CoeffOpt->number] = tempCol;
		CoeffOpt->value[CoeffOpt->number] = tempVal;
		CoeffOpt->number ++;

		//Vector
		double sum=0;
		for(j=1;j<=q2.size();j++)
		{
			sum=sum+prevGuess.getValue (q2.setArray [j-1],p)*
				getValue(q2.setArray [j-1],k);
		}
		rhsV[startingRow]= sum;
	}

	//************************************************************//

	//Call sparse 1.3 to solve the linear functions
	sparse13( CoeffOpt,rhsV,k, matrixArray);


	//return the result;
	nextGuess.zero ();
	nextGuess.setValue (k,k,rhsV[1]);
	for(i=0;i<Nk.size ();i++)
	{
		nextGuess.setValue (k,Nk.setArray [i],rhsV[i+2]);
		nextGuess.setValue (Nk.setArray [i],k,rhsV[i+2]);
	}

	//release memory
	free_vector(rhsV,1,n);
	delete CoeffOpt;
}



//for debug purpose
void
aMatrix::print(char* info)
{
	//test the return result
	cout << info << endl;
	for (long ww=1;ww<=n;ww++)
	{
		cout<<"-------------------------------------------------" <<endl;
		for (long i=1;i<=n;i++)
		{
			
			cout<< getValue (ww,i) << " , ";
		}
		cout << endl;
	}

	cout <<endl;

	return;
}


//Test if the linear algebraic equations are solved correctly
void test(double ** A,double *X,double *B,long n)
{
	cout << "Testing(Both sides equal?):" << endl;
	long i,m;
	double sum;
	for(i=1;i<=n;i++)
	{
		sum =0;
		for(m=1;m<=n;m++)
		{
			sum = sum +	A[i][m]*X[m];
		}
		cout <<  sum << " =? " << B[i-1] <<endl; 
	}
	cout <<endl;
}	


// Get the nearest nodes, return a set
Set<long> aMatrix::nearest(long k)
{	
	Set<long> result;
	double temp;
		
	for ( long column=1; column <= n; column++ )
	{
		temp = getValue(k,column);
		if( (temp != -1)&& (column !=k) )
		{
			result.insert(column);
		}
	}
	
	return result;
}

// update a matrix with (constant - elements) of the matrix
void
aMatrix::constantMinus(aMatrix &result,double constant)
{
	long n = result.n;
	for(long i=1;i<=n; i++)
	{
		for(long j=1;j<=n;j++)
		{
			result.setValue(i,j,(constant - getValue(i,j))); 
		}
	}
}

// matrixes addition
// result <--- result + a;
void 
aMatrix::plus(aMatrix &result,aMatrix &a)
{
	long n = result.n;
	double temp;
	for(long i=1;i<=n; i++)
	{
		for(long j=1;j<=n;j++)
		{
			temp = a.getValue (i,j)+getValue(i,j);
			result.setValue(i,j,temp); 
		}
	}
}


//matrix elements * constant
void 
aMatrix::multiply(aMatrix &result, double constant)
{
	double temp;

	for(long i=1;i<=result.n; i++)
	{
		for(long j=1;j<=result.n;j++)
		{
			temp = getValue (i,j);
			result.setValue(i,j,temp*constant); 
		}
	}
	
}


// Not standard cross product
// multiply corresponding element instead
void 
aMatrix::multiply(aMatrix &result, aMatrix &coeff)
{
	long n = result.n;
	for(long i=1;i<=n; i++)
	{
		for(long j=1;j<=n;j++)
		{
			result.setValue(i,j,getValue (i,j)*coeff.getValue (i,j)); 
		}
	}

}


//check if all the elements are 0's
bool 
aMatrix::IsZero()
{
	long temp;

	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			temp = getValue (row,column);
			if( temp != 0)
				return false;
		}
	}

	return true;
}


// return a matrix with -1 denoting vibration and 1 denoting not
void
aMatrix::vibrate(aMatrix &first, aMatrix &second, aMatrix &third)
{
	double v1,v2,v3;

	for(long i=1;i<=n; i++)
	{
		for(long j=1;j<=n;j++)
		{
			v1 = (first.getValue (i,j));
			v2 = (second.getValue (i,j));
			v3 = (third.getValue (i,j));

			// if not vibrating
			if ( (v2 - v1)*(v3 - v2) > 0)
			{
				setValue(i,j,1); 
			}
		}
	}

	return;
}

// if vibration is 1, means not vibrating, set alpha near 0
// else, set alpha = 3/4
// in order to make convergence faster
void
aMatrix::alphaUpdate(aMatrix &result)
{
	long n = result.n;
	for(long i=1;i<=n; i++)
	{
		for(long j=1;j<=n;j++)
		{
			// if not vibrating
			if ( getValue (i,j) == 1)
			{
				result.setValue(i,j,-0.7); 
			}
			//osilating
			else
			{
				result.setValue(i,j,0.75);
			}
		}
	}
}


/*
temp:
12 23	0
34	0	0
0	0	0


update this with temp
if this[][] is 0, this[][]=temp[][]
else average(this[][],temp[][])
*/
void
aMatrix::update(aMatrix &temp)
{
	for (long row=1; row <=n; row ++)
	{
		for ( long column=1; column <= n; column++ )			
		{//start for column
			double value = temp.getValue (row,column);
			if( value!=0)	
			{
				if(getValue(row,column)!=0)
				{
					setValue(row,column,(getValue(row,column)+value)/2);
				}
				else
				{
					setValue(row,column,value);
				}
			}
		}//end for column
	}
	return;

}

/*
change elements having value 0 in the matrix to -1 instead 
*/
void
aMatrix::negativeOne()
{
	for (long row=1; row <=n; row ++)
	{
		for ( long column=1; column <= n; column++ )			
		{//start for column
			double value = getValue (row,column);
			if( value==0)	
			{
				setValue(row,column,-1);
			}
		}//end for column
	}
	return;

}

//inverse a matrix
void
aMatrix::inverse(aMatrix &result)
{
	long n = result.n;
	double temp;
	// suppose the aMatrix elements can't be negative
	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			temp = getValue (row,column);
			if(temp == -1 )
			{
				result.setValue(row,column,-1);
			}
			else if( temp < 1e-12)
			{
			  temp = 1e-12;
			  // Do not crash on division by zero...
			  // Zero conductance is set to 1000 GOhm by default.
			}
			else
			{
				result.setValue(row,column,1/temp); //means unknow
			}
		}
	}

}


//if the corresponding elements of two matrixes are within delta
// 0 is set to denote the elements are deemed as same 
void 
aMatrix::SameUpdate(aMatrix &first, aMatrix &compare,double delta)
{
	double a,b,difference;

	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			a = first.getValue (row,column);
			b = compare.getValue (row,column);

			if((a+b)!=0)
			{
				difference = fabs ((a-b)/(a+b));
			}
			else
			{
				difference = 555; //just want the program continue converging
			}

			if( difference < delta)
			{
				setValue(row,column,0);
			}
			else
			{
				setValue(row,column,-1);
			}
		}
	}

	return;
}

// copy to matrix 'result'
void 
aMatrix::copy(aMatrix &result)
{
	long n = result.n;

	// suppose the aMatrix elements can't be negative
	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			result.setValue(row,column,getValue (row,column)); //means unknow
		}
	}
}

//Used in debug mode
//Calculate the average and maximum percentage difference of two matrixes
void
aMatrix::squareError(aMatrix &compare,double &average,double &max)
{
	double temp;

	double a1 = getValue(1,1);
	double a2 = compare.getValue (1,1);
	double divide;
	if( (a1+a2)!= 0 )
	{
		divide=(a1-a2)/(a1+a2);
	}
	else
	{
		divide =555;
	}
	
	double sum=0;
	double num = 0;

	max = fabs(divide);

	double temp1,temp2;

	for (long row=1; row <= n; row++)
	{
		for ( long column=1; column <= n; column++ )
		{
			if(getValue(row,column) != -1 )
			{
				temp1=getValue(row,column);
				temp2=compare.getValue (row,column);

				if((temp2+temp1)!=0)
				{
					temp= fabs((temp1-temp2)/(temp1+temp2));
				}
				else
				{
					temp=0;
				}
				
				num++;
				sum = sum + temp;
			
				if(temp > max) 
					max = temp;
			}
		}//end for loop
	}//end for loop

	average = sum/num;

	return;
}
