#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <stdlib.h>
#include "matrix.h"

using namespace std;

/****************************************
Global parameters geared to user's need
*****************************************/

//e.g. 500*500 matrix, for the purpose of spfactor in sptest.c
#define sizeOfMatrix 500
///////////////////////////////////////////



// open a file
void interactive_open( ifstream & fin, string & filename );

// output the result
void write(ofstream & fout, string &filename,aMatrix &result,long dimension);

// for testing purpose
void intermediateTest(ofstream & fout, string &filename,aMatrix &inter, long dimension);	
void intermediateTest(ofstream & fout, string &filename,double average,double max);	

double percentage(double previous, double current);

//read the file and set values to the aMatrix
void read(ifstream & fin, aMatrix & voltage);

//For the purpose of spfactor use in sptest.c
char * matrixArray[sizeOfMatrix];



int main(int argc, char *argv[])
{
	////////////////////////////////////////////////////////////////
	// make precision smaller to get even accurate result
	// can be read as a parameter
	// must use float to sscanf with "f" parameter

	float precision=1.0e-4; 

	// Gi+1 = ALPHA * Gi + (1-ALPHA) * G~
	//from experience, default value
	float ALPHA=0.7;

	int OsillationPeriod= 2; //default value
	int ConstantAlphaPeriod= 4;//default value
	////////////////////////////////////////////////////////////////
	
	
	//Initialize the matrixes and store them
	//so no need to call spOrderAndFactor() again and again
	for ( int tmp = 0; tmp < sizeOfMatrix; tmp++)
	{
		matrixArray[tmp]= 0;
	}

	//get the input file name
	ifstream fin;
	ofstream fout;
	ofstream ftest;

	string filename;
	string writeFileName="r.out";
	string testFileName = "error.out";

	//Part I
	
	int argi;  /*agri will be used to count the number of elements in the command line*/
	/*argc is the number of elements in the command line*/
	/*argv is an array of character pointers*/

    printf("Usage: %s [options]  \n\n",argv[0]);
    printf("   options:\n");
    printf("   -precision= #   to specify the precision\n");
    printf("   -alpha= #       constant alpha value for convergence purpose\n");
	printf("   -op= #          oscillation algorithm period\n");
	printf("   -cp= #          constant alpha value algorithm period\n");
	printf("   filename        to specify the input file\n");
	printf("   -debug          print out some debug information\n");
    printf(" \n");
	

/*parse the command string*/
/*uses sscanf and strncmp which are functions provided by c*/
/*man sscanf and man strncmp will bring up their man pages*/

	bool debug = 0;

	bool hasfilename = 0;
	bool alphaRead=0;

	double previousAverage=0;
	double currentAverage=0;

	int maxIteration = 100;

	if (argc <= 1) // without parameter
	{
		interactive_open( fin, filename );
	}
	else
	{
		  argi=1; /*set to one since the first element is the name of the program*/

		  /*loop over all elements after 1*/
		  for (int i = 1; i < argc; ++i) 
		  {			
			  //1.option -precision=
				if (0 == strncmp(argv[i], "-precision=", 11))
				{
	    			 argi=argi+2;
					  // condition check for the # of arguments is less than neededs
      				if ( argi> argc){
  					    printf("\nParameter is not recognized\n\n");
						return 0;
     				}
      				sscanf(argv[++i],"%f",&precision);           /*read in the next arguement, a number*/

      				
      				if (precision<=0)
					  {
						 printf("\nprecision must be positive!\n\n");
						 return 0;
					  }

				  }


				//2.option -alpha=
				else if (0 == strncmp(argv[i], "-alpha=", 7))
				{
					 argi=argi+2;
					  // condition check for the # of arguments is less than neededs
      				if ( argi> argc){
  				   printf("\nParameter is not recognized\n\n");
				   return 0;
     				}
      				sscanf(argv[++i],"%f",&ALPHA);           /*read in the next arguement, a number*/

      				
      				if (ALPHA<=0)
					  {
						 printf("\nalpha must be positive!\n\n");
						 return 0;
					  }

					alphaRead=1;
				  }

				//3.option -op=
				else if (0 == strncmp(argv[i], "-op=", 4))
				{
					 argi=argi+2;
					  // condition check for the # of arguments is less than neededs
      				if ( argi> argc){
  				   printf("\nParameter is not recognized\n\n");
				   return 0;
     				}
      				sscanf(argv[++i],"%d",&OsillationPeriod);           /*read in the next arguement, a number*/

      				
      				if (OsillationPeriod<=0)
					  {
						 printf("\nop must be positive!\n\n");
						 return 0;
					  }
				  }
				//4.option -cp=
				else if (0 == strncmp(argv[i], "-cp=", 4))
				{
					 argi=argi+2;
					  // condition check for the # of arguments is less than neededs
      				if ( argi> argc){
  				   printf("\nParameter is not recognized\n\n");
				   return 0;
     				}
      				sscanf(argv[++i],"%d",&ConstantAlphaPeriod);           /*read in the next arguement, a number*/

      				
      				if (ConstantAlphaPeriod<=0)
					  {
						 printf("\ncp must be positive!\n\n");
						 return 0;
					  }
				  }
				//5. option -debug
				else if (0 == strncmp(argv[i], "-debug", 6))
				{
					 argi=argi+1;
					  // condition check for the # of arguments is less than neededs
      				if ( argi> argc){
  				   printf("\nParameter is not recognized\n\n");
				   return 0;
     				}
					debug = true;

				  }
				//6. option -maxiter=
				else if (0 == strncmp(argv[i], "-maxiter=", 9))
				{
					 argi=argi+2;
					  // condition check for the # of arguments is less than neededs
      				if ( argi> argc){
  				   printf("\nParameter is not recognized\n\n");
				   return 0;
     				}
      				sscanf(argv[++i],"%d",&maxIteration);           /*read in the next arguement, a number*/

      				
      				if (maxIteration<=0)
					  {
						 printf("\ncp must be positive!\n\n");
						 return 0;
					  }
				  }

				else	
				{
					argi=argi+1;

					filename = argv[i];
					fin.open( filename.c_str() );

					 // If the file is not valid
					if( ! fin.is_open() )
					{
						cout << argv[i] << " is not a valid file!  " <<endl;
						exit(1);
					}

					hasfilename=1;
				}

		  }//end of for loop

		  /*see if all the elements in the command string could be accounted for*/
		  /*if not print usage message and return*/
			if (argi != argc) {
				cout << "Parameter error!" << endl;
				exit(1);
			}
			if(hasfilename==0)
			{
				interactive_open( fin, filename );
			}


	}


	//Part II *******************

	//Read the first line which determines the dimension of the aMatrix
	long dimension;
	fin >> dimension;

	if(fin.eof())
	{
		cout << "The file is empty " << endl;
		exit(1);
	}

	//Read the rest part of the input file
	aMatrix voltage(dimension);
	read(fin,voltage);

	//Make the voltage aMatrix symmetric
	// if the value is -1, then ignore.
	voltage.symmetry();

	//test the reading file is correct
//	voltage.print ("input");

	//Initialize for the first iteration
	aMatrix result(dimension);
	long i,j;
	for(i=1;i<=dimension;i++)
	{
		result.setValue (i,i,1.0/voltage.getValue (i,i));
		Set<long> adjacent = voltage.nearest (i);


		for(j=1;j<=adjacent.size ();j++)
		{
			// if (-1) means, the two nodes are not connected
			result.setValue (adjacent.setArray [j-1],i,0);
			result.setValue (i,adjacent.setArray [j-1],0);
		}
		
	}

	// make the first guess reasonable
	result.rationalize ();

	//alpha matrix
	aMatrix alpha(dimension);
	alpha.multiply (alpha, -ALPHA);
	aMatrix complimentAlpha(dimension);


	// Same matrix, denoting element need not to be changed.
	// -1 means need to change, 0 otherwise.
	aMatrix same(dimension); 
	aMatrix backup(dimension);
	aMatrix temp(dimension);
	aMatrix oddAverage(dimension);
	aMatrix transient(dimension);
	aMatrix nextG(dimension);
	aMatrix IsVibrating(dimension);
	aMatrix temp1(dimension);

	aMatrix coeff(dimension);

	long counter = 0;
	float tempAlpha = ALPHA;
	int switchNumber = 0;

	double average=0,max=0;
	double change=1;

	int period= OsillationPeriod+ConstantAlphaPeriod;
	int module;


	if (debug) {
	  ftest.open(testFileName.data());

	  if( ! ftest.is_open() )   cout << "Error opening writing-file." <<endl;
	  ftest << "# 1st element" << "    "
		<< "Average % diff" 
		<<"    " << "Max % diff"<< endl;
	}


	// begin looping and searching for next guess
	for(counter=0; counter<maxIteration; counter++)
	{		
		temp.zero ();

		for(i=1;i<=dimension;i++)
		//for(i=dimension;i>=1;i--)
		{		
			coeff.initialize ();
			voltage.guess(temp1,result,dimension,i,matrixArray);
			temp1.copy(transient);
			temp.update (transient); // update temp with transient
		}

		temp.negativeOne ();

		//Make temp values which make sense
		temp.rationalize ();

	// calculate "expected" next guess
	// will be revised according to vibration

		// Gi+1 = ALPHA * Gi + (1-ALPHA)* G~
		//              result           temp
		if(alphaRead==0)
		{
			module= counter%period;

			//first use constant alpha value algorithm
			if(module < ConstantAlphaPeriod)
			{
				result.copy(nextG);
				nextG.multiply (nextG, tempAlpha);	
				temp.multiply ( temp1, 1.0-tempAlpha);
				nextG.plus ( nextG,temp1 );
			}
			else
			{
				// test vibrating
				IsVibrating.minusOne ();
				IsVibrating.vibrate(backup,result,temp);
				IsVibrating.alphaUpdate (alpha);

				alpha.constantMinus (complimentAlpha, 1 );

				//Then the next guess is:
				result.multiply(nextG,alpha);
				temp.multiply (temp1,complimentAlpha);
				nextG.plus (nextG,temp1);
				nextG.rationalize ();
			}
		}
		else // if constant alpha all the time,use this algorithm
		{
			result.copy(nextG);
			nextG.multiply (nextG, tempAlpha);	
			temp.multiply ( temp1, 1.0-tempAlpha);
			nextG.plus ( nextG,temp1 );
		}
			
		result.copy (backup);


		//for debug purpose
		if(debug)
		{
			nextG.print("NextG is:");

			//testing the convergence of the first element of the result
			intermediateTest(ftest, testFileName,nextG,dimension);
			
			result.squareError(nextG,average,max);
			change=percentage(average,previousAverage);
			previousAverage=average;

			intermediateTest(ftest, testFileName,average,max);
		}


		same.SameUpdate( nextG,result,precision);

		if( same.IsZero() || counter == maxIteration-1) // got result
		{			
			aMatrix inver(dimension);
			result.inverse(inver);
			inver.print("Inverse R ==");

			// output the result
			write(fout,writeFileName,inver,dimension);
			break;
		}
		else //loop again
		{
			nextG.copy (result);
		}

	}


	cout << "# of iterations: " << counter <<endl;

	if(counter == maxIteration)
		cout << "The maximum iteration number is too small for this case" << endl
		<< " or it couldn't converge at all" << endl;

	if (debug) ftest.close();
	fin.close();
	fout.close();
	return 0;
}




//Functions
void read(ifstream & fin,aMatrix &voltage)
{
	long row; 
	long column;
	double value;
	for(;;)
	{
		fin >> row >> column >> value;
		if(fin.eof()) break;
		
		voltage.setValue (row,column,value);

	}

	return;
}


// The function get the name of the input file.
void interactive_open( ifstream & fin, string & filename )
{
  // Get the filename from the user
  cout << "Please insert the name of the input file: ";
  cin  >> filename;
  fin.open( filename.c_str() );

  // If the file is not valid, prompt again
  while( ! fin.is_open() )
    {
      cout << "This is not a valid file!  " <<endl
           << "Please insert the name of the input file:";

      cin  >> filename;
      fin.open( filename.c_str() );
	  fin.clear();
    }
}


void write(ofstream & fout, string &filename,aMatrix &result,long dimension)
{
	fout.open(filename.data());
	double temp;
	long i,j;

	if( ! fout.is_open() )   cout << "Error opening writing-file." <<endl;
	else 
	{
		for(i=1;i<=dimension; i++)
		{
			for(j=i+1;j<=dimension;j++)
			{
				temp = result.getValue (i,j);
				if ( temp != -1)
					fout << i <<" " << j << " " <<temp <<endl; 
			}
		}

		for( i=1; i<=dimension; i++)
		{
			temp = result.getValue (i,i);
			fout << i << " 0 " << temp <<endl;
		}

	}	
}

void intermediateTest(ofstream & fout, string &filename,aMatrix &inter,long dimension)
{
	if( ! fout.is_open() )   cout << "Error opening writing-file." <<endl;
	else 
	{ 
		//test the element[1][1] of the result
		fout<< inter.getValue (1,1) <<"    "; // << endl;
	}	
}
void intermediateTest(ofstream & fout, string &filename,double average,double max)
{
	if( ! fout.is_open() )   cout << "Error opening writing-file." <<endl;
	else 
	{
		// average, then max
		fout<< average <<"    " << max << endl;
	}	
}

double percentage(double previous, double current)
{
	double temp;
	if((previous+current)!=0)
	{
		temp = (previous-current)/(previous+current);
	}
	else
	{
		temp=0;
	}
	temp = fabs(temp);
	return temp;
}
