# Leeward-Cplusplus

Leeward: a simple inductance sparsification program

Peiyan(Rocky) Wang

1 Introduction
Traditionally, to model inductance effects in circuits involves of defining a dense matrix of all the interaction coefficients among all wires, or partial inductance matrix. It incurs the complexity problem of solving non-linear functions. In order to simplify the problem, a couple of techniques have been developed to sparsify the partial-inductance matrix. This document describes the Leeward program, which is based on such a simple inductance sparsification technique [1].

2 Environments
Solaris, Linux, Windows

3 Usage
The program takes a voltage matrix as input, outputs the resistance matrix.

Make a simple example for better understanding, suppose the circuit is a square where each node is connected to its neighbor by a 100ohm resistor, and another 100 ohm to
ground.

The input voltage matrix file looks like the following:

4
1 1 93.333
2 2 93.333
3 3 93.333
4 4 93.333
1 2 40
2 3 40
3 4 40
1 4 40

The first line denotes the circuit has 4 nodes. The second line means that the voltage of node 1 is 93.333. The sixth line means node 1 and node 2 are connected and the voltage is 40. 

The programs outputs result into output.doc file. It has such a format:

1 2 100
2 3 100
3 4 100
4 1 100
1 0 100
2 0 100
3 0 100
4 0 100

For example, the first line means the resistance between node 1 and node 2 is 100ohm. The fifth line means the resistance between node 1 and the ground is 100ohm.

_

4 Algorithms
Leeward is written in C++ programming language. The linear equations solver is sparse 1.3, which was developed by Kenneth S. Kundert and the University of California.

4.1 Data Objects
Set class           : support subset, union, intersection, difference and quicksort
Matrix class     : support most of the operations needed by the main program
CompactSparse: a compact representation of matrix 

4.2 Parameters
options:
 -precision= #   to specify the precision
 -alpha= #         constant alpha value for convergence purpose
 -op= #              oscillation algorithm period
 -cp= #              constant alpha value algorithm period
 filename           to specify the input file
-debug               print out some debug information

If no parameters are provided, the program uses default values and prompt the user to input the file name.

If –alpha option is used, the program applies constant alpha value algorithm only.
If else, Leeward first applies constant alpha value algorithm within the period which option –cp has specified, then applies oscillation algorithm within the period which option –op has specified.

This procedure continues until either the program has converged or the maximum iteration number is reached.

The debug option is used to trace the execution history of the program. It outputs the next guess to the screen, and other information in inter.doc.

4.3 Global Variables
In leeward.cpp,
MaxIteration : Maximum iteration number
SizeOfMatrix: The maximum dimension of the input matrix

4.4 Program flow chart











First an initial guess is made. Then calculate the circuit according to the initial guess and the voltage matrix. Make a reasonable next guess according to the first guess and the calculated circuit. This is based on Gi+1 = ALPHA * Gi + (1-ALPHA)* G~. 

Here Gi+1: next guess; Gi : previous guess; G~: the calculated circuit; ALPHA: a constant.

The procedure continues so on and so forth until two adjacent guesses are within precision requirement.

As to the oscillation algorithm, the program keeps track of the history of latest three values for each element in the matrix. If oscillation is found, alpha is set to be 0.75 which
makes the next guess more reasonable by averaging the previous guesses. And if not, alpha is set to be –0.7 from experience which leads the next guess go to the same direction as the previous guesses do.

The basic principle is that the program tries to iterate with constant alpha value for a couple of times, then use oscillation algorithm to detect the converging situation and adjust and improve next guesses, then iterate with constant value again and diagnose again. 

The precision option defines percentage change of two adjacent guesses and it is the convergence condition.

Sparse 1.3 linear equations solver is used for fast and general uses. 

4.5 Minor tricks
If some guess is less than zero, the program clips it and sets it to zero instead. If it is bigger than one, the program sets it to one instead. This shows that the resistance is between one ohm and infinity. If the user needs to expand the limit, change the corresponding value in function “rationalize”. 
4.6 Major functions

1.
/**********************************************************************
Functionality: Use prevGuess aMatrix, long k and Voltage aMatrix to calculate the nextGuess aMatrix
Parameters   :  prevGuess is a G-aMatrix
Output	       :  nextGuess is the returned UPDATED G-aMatrix
**********************************************************************/
void aMatrix::guess(aMatrix &nextGuess, aMatrix &prevGuess, long dimension, long k,	
char ** matrixArray)
2.
/**********************************************************************
Functionality: Given coefficient matrix and right vector, solve the linear equations
Parameters   : Coefficient matrix, right vector, the node being considered, the reusable linked-list which stores the coefficient matrix
Output          : The result is written back into the right vector
**********************************************************************/
void sparse13(struct compactSparse * coeff, double *RVector,long k,
			  char ** matrixArray)

5 CPU Time Pie Chart

Based on 100 nodes, 10 neighbors and dynamics = 1. Data is generated with gprof:

 
6 List Of Files
common.h	compactSparse struct
matrix.h	matrix class header
matrix.cpp	matrix class implementation
set.h		set class header
set.cpp		set class implementation
nrutil.h		linear equations solver header
nrutil.cpp	linear equations solver implementation
leeward.cpp	main program
makefile	makefile for Linux  and Solaris


Sparse 1.3 package:
spdefs.h				spconfig.h
spmatrix.h				sputils.c
spsolve.c				spoutput.c
spfactor.c				spbuild.c
spallocate.c				sptest.c


The program generates inter.doc for debug uses and output.doc for storing the result.

The first column in inter.doc records the convergence history of the first element in G matrix.

The second column is the average percentage change of two adjacent guesses for all G matrix elements.

The third column is the maximum percentage change of two adjacent guesses for all G matrix elements.


References
[1] A. Pacelli, “A simple inductance sparsification algorithm”, submitted to IEEE/ACM International Conference on Computer Aided Design.
[2] A. Pacelli, H. Yu, S. Shevde, L. Wang, and P. Wang, “Vector-potential equivalent circuit (VPEC) for
inductance modeling”, Tech. Rep., State University of New York, Stony Brook
[3] Numerical Recipe
