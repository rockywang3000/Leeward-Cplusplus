/****************************************************
Changes made to the original sptest.c which was written by Kenneth S. Kundert

Three major functions:
1. //read in coefficient matrix and right vector
//the linked structure are created
int read(struct compactSparse* coeff,double *RVector, long k,
		 char ** matrixArray);

2. //the linked structure are the same
//read in the coefficient matrix and right vector to the linked structure
int read2(struct compactSparse* coeff,double *RVector, long k,
		 char ** matrixArray);

3.//export to be used
//parameters: coefficient matrix, right vector, the current node considered
//return the solution in the parameter list
void sparse13(struct compactSparse* coeff,double *RVector,long k,
			  char ** matrixArray);

*****************************************************/
#ifndef lint
static char copyright[] =
    "Sparse1.3: Copyright (c) 1985,86,87,88 by Kenneth S. Kundert";
static char RCSid[] =
    "@(#)$Header: spTest.c,v 1.2 88/06/18 11:16:24 kundert Exp $";
#endif



/*
 *  IMPORTS
 *
 *  >>> Import descriptions:
 *  stdio.h math.h ctype.h
 *      Standard C libraries.
 *  spConfig.h
 *      Macros that customize the sparse matrix package. It is not normally
 *      necessary, nor is normally particularly desirable to include this
 *      file into the calling routines.  Nor should spINSIDE_SPARSE be defined.
 *      It is done in this test file so that the complex test routines may be
 *      removed when they don't exist in Sparse.
 *  spmatrix.h
 *      Macros and declarations to be imported by the user.
 */

#define  YES    1
#define  NO     0

#include <stdio.h>
#include <math.h>
#include <ctype.h>
#define spINSIDE_SPARSE
#include "spconfig.h"
#undef spINSIDE_SPARSE
#include "spmatrix.h"
#include "common.h"

/* Declarations that should be in stdio.h. */
extern char *malloc();
#ifdef ultrix
    extern void free();
    extern void exit();
    extern void perror();
#else
    extern free();
    extern exit();
    extern void perror();
#endif

/* Declarations that should be in strings.h. */
extern int strcmp(), strncmp(), strlen();

#ifdef lint
#undef  MULTIPLICATION
#undef  DETERMINANT

#define  MULTIPLICATION                 YES
#define  DETERMINANT                    YES

#define LINT    YES
#else /* not lint */
#define LINT    NO
#endif /* not lint */


/*
 *  MACROS
 */

#define  BOOLEAN  int
#define  NOT  !
#define  AND  &&
#define  OR  ||
#define  ALLOC(type,number) ((type *)malloc((unsigned)(sizeof(type)*(number))))
#define  ABS(a)         ((a) < 0.0 ? -(a) : (a))
#ifdef MAX
#undef  MAX
#endif
#ifdef MIN
#undef  MIN
#endif
#define  MAX(a,b)       ((a) > (b) ? (a) : (b))
#define  MIN(a,b)       ((a) < (b) ? (a) : (b))


/*
 *  IMAGINARY VECTORS
 *
 *  The imaginary vectors iRHS and iSolution are only needed when the
 *  options spCOMPLEX and spSEPARATED_COMPLEX_VECTORS are set.  The following
 *  macro makes it easy to include or exclude these vectors as needed.
 */


#define IMAG_VECTORS


/* Begin global declarations. */
typedef  spREAL  RealNumber, *RealVector;

static int  Size, MaxSize = 0;
static BOOLEAN DiagPivoting = DIAGONAL_PIVOTING;
static RealNumber AbsThreshold = (RealNumber)0, RelThreshold = (RealNumber)0;
static RealVector RHS, Solution, RHS_Verif;
int Init();
RealVector CheckOutComplexArray( int Count );
void EnlargeVectors( int NewSize, int Clear );
void CheckInAllComplexArrays();

//read in coefficient matrix and right vector
//the linked structure are created
int read(struct compactSparse* coeff,double *RVector, long k,
		 char ** matrixArray);

//the linked structure are the same
//read in the coefficient matrix and right vector to the linked structure
int read2(struct compactSparse* coeff,double *RVector, long k,
		 char ** matrixArray);

//export to be used
//parameters: coefficient matrix, right vector, the current node considered
//return the solution in the parameter list
void sparse13(struct compactSparse* coeff,double *RVector,long k,
			  char ** matrixArray);



/********************************************************
			Implementations
********************************************************/

/*******************************************************
Parameters: coefficient matrix, right vector, 
			the node is considered, 
			the reusable linked-list which stores the coefficient matrix
return: the result is written back into the right vector
*********************************************************/
void sparse13(struct compactSparse * coeff, double *RVector,long k,
			  char ** matrixArray)
{
	register  long I;
	int  Error;

	if (matrixArray[k] != 0 ) // reuse the matrix, only call spFactor()
	{
		Error = read2(coeff, RVector,k, matrixArray);

		(void)spInitialize(matrixArray[k], Init);
	
        Error = spFactor( matrixArray[k] );
        if( Error >= spFATAL ) goto End;
        
		spSolve( matrixArray[k], RHS, RVector IMAG_VECTORS );
	}
	else	/* Create matrix. call spOrderAndFactor()*/
	{
        matrixArray[k] = spCreate( 0, spCOMPLEX, &Error );
        if (matrixArray[k] == NULL)
        {   printf("insufficient memory available.\n");
            exit(1);
        }
        if( Error >= spFATAL ) goto End;

/* Read matrix. */
		Error = read(coeff, RVector,k, matrixArray);

/* Perform initial build, factor, and solve. */
        (void)spInitialize(matrixArray[k], Init);

        Error = spOrderAndFactor( matrixArray[k], RHS, RelThreshold, AbsThreshold,
                                  DiagPivoting );
        if( Error >= spFATAL )    goto End;
	
		spSolve( matrixArray[k], RHS, RVector IMAG_VECTORS );
	}

End:;
        (void)fflush(stdout);
        CheckInAllComplexArrays();
}





/*
 *   READ MATRIX
 *
 *   This function reads the matrix and the RHS vector.
 *   >>> Returned:
 *   The error status is returned.  If no error occurred, a zero is returned.
 *   Otherwise, a one is returned.
 *
 *   >>> Local variables:
 *   >>> Global variables:
 *   Size  <set>
 *   Element  <set>
 *   RHS  <set>
 */

 static int read(struct compactSparse* coeff,double *RVector, long k,
	 char ** matrixArray)
 {
	int Row, Count = 0;
	RealNumber *pValue, *pInitInfo, *CheckOutComplexArray();
	RealNumber *pElement;

	Size = coeff->n;
    EnlargeVectors( Size, YES );

/* Read matrix elements. */
  if (Count == 0)
            pValue = CheckOutComplexArray( Count = 10000 );
	
  for (Row = 0; Row < coeff->number; Row++)
	{
		pElement = spGetElement( matrixArray[k], coeff->row[Row], coeff->column[Row] );
		if (pElement == NULL)
		{   printf("insufficient memory available.\n");
			exit(1);
		}
		pInitInfo = (RealNumber *)spGetInitInfo( pElement );
		if (pInitInfo == NULL)
		{  
			pValue[--Count] = coeff->value[Row];
			spInstallInitInfo( pElement, (char *)(pValue + Count) );
		}
	}

    Size = spGetSize( matrixArray[k], YES );

	for(Row=1;Row<=coeff->n;Row++)
	{
		RHS[Row] = RVector[Row];
	}

    return 0;
 }



 /*
 *   INITIALIZE MATRIX ELEMENT
 *
 *   This function copys the InitInfo to the Real and Imag matrix element
 *   values.
 *
 *   >>> Returned:
 *   A zero is returns, signifying that no error can be made.
 */

/*ARGSUSED*/

static int
Init( pElement, pInitInfo, Row, Col)

RealNumber *pElement;
char *pInitInfo;
int Row, Col;
{
/* Begin `Init'. */
    *pElement = *(RealNumber *)pInitInfo;

    return 0;
}


/*
 *  REAL ARRAY ALLOCATION
 *
 *  These routines are used to check out and in arrays of real numbers.
 */

static struct Bin {
    RealVector   Array;
    struct Bin     *Next;
}  *FirstArray, *CurrentArray = NULL;


static RealVector
CheckOutComplexArray( Count )

int Count;
{
struct Bin Bin, *pBin;
RealVector Temp;

/* Begin `CheckOutComplexArray'. */

    if (CurrentArray == NULL OR CurrentArray->Next == NULL)
    {   pBin = ALLOC( struct Bin, 1);
        Temp = ALLOC( RealNumber, Count );
        if (pBin == NULL OR Temp == NULL)
        {   printf( "insufficient memory available.\n");
            exit(1);
        }
        Bin.Array = Temp;
        Bin.Next = NULL;
        *pBin = Bin;
        if (CurrentArray == NULL)
            FirstArray = CurrentArray = pBin;
        else
        {   CurrentArray->Next = pBin;
            CurrentArray = pBin;
        }
    }
    else
    {   pBin = CurrentArray;
        CurrentArray = pBin->Next;
    }

    return pBin->Array;
}


static void CheckInAllComplexArrays()
{

/* Begin `CheckInAllComplexArrays'. */
    if (CurrentArray != NULL)
        CurrentArray = FirstArray;
    return;
}

/*
 *  ENLARGE VECTORS
 *
 *  Allocate or enlarge vectors.
 */

static void
EnlargeVectors( NewSize, Clear )

int NewSize, Clear;
{
int I, PrevSize = MaxSize;
RealVector OldRHS = RHS;
#if spCOMPLEX
#   define SCALE 2
#else
#   define SCALE 1
#endif

/* Begin `EnlargeVectors'. */
    if (NewSize > PrevSize)
    {   if (MaxSize != 0)
        {   free( (char *)Solution );
            free( (char *)RHS_Verif );
        }
        RHS = ALLOC( RealNumber, SCALE*(NewSize+1) );
        Solution = ALLOC( RealNumber, SCALE*(NewSize+1) );
        RHS_Verif = ALLOC( RealNumber, SCALE*(NewSize+1) );

/* Copy data from old RHS to new RHS. */
        if (NOT Clear)
        {
/* Copy old RHS vector to new. */
          for (I = PrevSize; I > 0; I--)
                    RHS[I] = OldRHS[I];

        }
        if (MaxSize != 0) free( (char *)OldRHS );
        MaxSize = NewSize;
    }

/* Either completely clear or clear remaining portion of RHS vector. */
    if ((NewSize > PrevSize) OR Clear)
    {   if (Clear)
        {   NewSize = MAX( NewSize, PrevSize );
            PrevSize = 0;
        }

 
       for (I = NewSize; I > PrevSize; I--)
                RHS[I] = 0.0;
    }

    return;
}


 static int read2(struct compactSparse* coeff,double *RVector, long k,
	 char ** matrixArray)
 {
	int Row, Count = 0;
	RealNumber *pValue, *pInitInfo, *CheckOutComplexArray();
	RealNumber *pElement;

/* Read matrix elements. */
  if (Count == 0)
            pValue = CheckOutComplexArray( Count = 10000 );

	for (Row = 0; Row < coeff->number; Row++)
	{
		pElement = spGetElement( matrixArray[k], coeff->row[Row], coeff->column[Row] );
		if (pElement == NULL)
		{   printf("insufficient memory available.\n");
			exit(1);
		}

		
		pValue[--Count] = coeff->value[Row];
		spInstallInitInfo( pElement, (char *)(pValue + Count) );
	
	}

	for(Row=1;Row<=coeff->n;Row++)
	{
		RHS[Row] = RVector[Row];
	}

    return 0;
 }