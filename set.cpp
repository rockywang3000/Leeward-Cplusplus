#include "set.h"
#include <iostream>
#include <cstdlib>  // to use exit(1);
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


using namespace std;

// 2 functions which are needed for calling qsort library
static int compkey(void *i, void *j);
static int compd(void *i, void *j);

template <class type>     // this line should be before any template function
void Set<type>::sort()
{

	// Pay attention to the type being used !!!
	qsort((void *)setArray, size(), sizeof(type), (int (*)(const void*,const void *))compd);

	//	QuickSort(0, size()-1);
	
}


static int compd(void *i, void *j) {

	// Pay attention to the type being used !!!
  long *p,*q;

  p = (long *)i;
  q = (long *)j;

  if (*p > *q) return 1; /* to decreas order, change sign here */
  if (*p < *q) return -1;
  return 0;
}


//Handwritten quicksort functions
template <class type>
void 
Set<type>::QuickSort(int p, int r)
{
	int iii, q;
	
	if (p >= r) return;
	iii = (p+r)/2;
	//TRACE("%d\n", iii);


	Swap(setArray[iii], setArray[r]);
	q = Partition(p, r);
	QuickSort(p, q-1);
	QuickSort(q+1, r-1);
}

template <class type>     // this line should be before any template function
int 
Set<type>::	Partition(int p, int r)
{
	type x = setArray[r];
	int iii = p-1;
	for (int jjj = p; jjj <= r-1; jjj ++)
	{
		if (setArray[jjj] <= x)
		{
			iii ++;
			Swap(setArray[iii], setArray[jjj]);
		}
	}
	Swap(setArray[iii+1], setArray[r]);
	return iii+1;
}

template <class type>     // this line should be before any template function
void 
Set<type>::Swap(type &e1, type &e2)
{
	type temp;

	temp = e1;
	e1 = e2;
	e2 = temp;
}
//////////////////////////////////////////////////////////////////



//constructor
template <class type>     // this line should be before any template function
Set<type>::Set()
{
	position=-1;  //initialization
}


/********************************************************
* Input: A=B=C
* function: make A=C and B=C;
* return type: Set &
*********************************************************/
template <class type>
Set<type> & 
Set<type>:: operator = (const Set<type> &set) // assignment
{
	// A=B;
	for(long i=0;i<=set.position;i++)
	{
		setArray[i]=set.setArray[i];
	}

	position=set.position;

	return *this;
}

/********************************************************
* Input: type item
* function: insert item into the Set,then position++  
* return type: bool
*********************************************************/
template <class type>
bool Set<type>::insert(const type item)
{
	bool result=false;
	if(position!=MAX_SIZE-1)
	{
		
		for( long i=0;i<=position;i++)
		{
			if (setArray[i]==item) return result;
		}
		position++;
		setArray[position]=item;
		result=true;
		return result;
	}
	return result;
}

/********************************************************
* Input: nothing
* output: the size of the set 
* return type: long
*********************************************************/
template <class type>
long Set <type>::size()
{
	return position+1;
}


/********************************************************
* Input: type item
* function: remove the item from the Set  
* return type: bool
*********************************************************/
template <class type>
bool Set<type>::remove(const type item)
{
	bool result=false;
	for(long i=0;i<=position;i++)
	{
		if(setArray[i]==item)
		{
				for(long m=i;m<position;m++)
				{
					setArray[m]=setArray[m+1];
				}

			position--;
			result=true;
			return result;
		}
	}

	return result;
}

/********************************************************
* Input: nothing
* function: empty the set
* return type: void
*********************************************************/
template <class type>
void Set<type>::clear()
{
	position=-1;
}


/********************************************************
* Input: A+B
* output: C= A U B
* return type: Set
*********************************************************/
template <class type>
Set<type> Set<type>::operator+   (const Set<type> &set) const  // union
{
	if((position+1+ set.position+1)<=MAX_SIZE)
	{
		Set<type> plus;

		for(long i=0;i<(position+1);i++)
		{
			plus.insert(setArray[i]);
		}
		for(long m=0;m<(set.position+1);m++)
		{
			plus.insert(set.setArray[m]);
		}

		return plus;
	}

	else
	{
		cout << "ERROR - OVERFLOW" <<endl;
		exit(1); // It doesn't matter to use exit(0) or exit(1)
		         // any return value other than 0 means unsuccessful termination
	}
}

/********************************************************
* Input: A&&B
* output: C= A ^ B  intersection
*return type: Set
*********************************************************/
template <class type>
Set<type>
Set<type>::operator &&  (const Set &set) const  // intersection // one more Set<type>???
{
	Set<type> Intersection;

	for(long i=0;i<(position+1);i++)	
	{	
		for(long m=0;m<(set.position+1);m++)
		{
			if(setArray[i]==set.setArray[m])
			{
				Intersection.insert(setArray[i]);
				break;
			}
		}
		
	}

	return Intersection;
}

/********************************************************
* Input: A-B
* output: C= A - B  difference
* return type: Set 
*********************************************************/
template <class type>
Set<type>
Set<type>::operator -   (const Set &set) const  // difference // outside const means "this" is const
{
	Set<type> difference;
	long m;

	for(long i=0;i<(position+1);i++)	
	{	
		for(m=0;m<(set.position+1);m++)
		{
			if(setArray[i]==set.setArray[m])
			{		
				break;
			}
		}

		if (m==(set.position+1))
		{
			difference.insert(setArray[i]);
		}
	}

	return difference;

}

/********************************************************
* Input: A<B
* output: C= A < B  proper subset
* return type: bool
*********************************************************/
template <class type>
bool
Set<type>::operator <   (const Set &set) const // proper subset
{
	bool is_proper=false;
	long counter;

	if(position>=set.position)
	{
		return is_proper;
	}
	else
	{

		for(long i=0;i<(position+1);i++)	
		{	
			for(counter=0;counter<(set.position+1);counter++)
			{
				if(setArray[i]==set.setArray[counter])
				{
					break;
				}
			}
			if(counter==(set.position+1))
			{
				return is_proper;
			}

		}

		is_proper=true;
		return is_proper;

	}


	
}


/********************************************************
* Input: A<=B
* output: subset
* return type: bool
*********************************************************/
template <class type>
bool        
Set<type>::operator <=  (const Set &set) const // subset
{
	bool is_subset=false;
	long counter;

	if(position>set.position)
	{
		return is_subset;
	}
	else
	{

		for(long i=0;i<(position+1);i++)	
		{	
			for(counter=0;counter<(set.position+1);counter++)
			{
				if(setArray[i]==set.setArray[counter])
				{
					break;
				}
			}
			if(counter==(set.position+1))
			{
				return is_subset;
			}

		}

		is_subset=true;
		return is_subset;

	}


}


/********************************************************
* Input: A==B
* function: equality 
* return type: bool
*********************************************************/
template <class type>
bool        
Set<type>::operator ==  (const Set &set) const // equality
{
	return( (*this<=set)&&(set<=*this) );
}


/********************************************************
* Input: cout<<A
* output: the elements of set A 
* return type: ostream&
*********************************************************/
template <class type>
ostream &     operator << (ostream &out, const Set<type> &set) //note how to use "Set<type>
{
	if(set.position ==-1)
	{
		out << "Empty Set" <<endl;
		
	}
	else
	{
		for(long counter=0;counter<(set.position +1);counter++)
		{
			out << set.setArray [counter] << " " <<flush;
		}
		out << endl;
	}

	return out;

}

