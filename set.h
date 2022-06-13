const long MAX_SIZE = 700;  //Max set size

#include <iostream>   // necessary

using namespace std;

template <class type> 

class Set
{

  public:

                Set();  // constructor

    Set &       operator =   (const Set &set); // assignment

    bool        operator <   (const Set &set) const; // proper subset
    bool        operator <=  (const Set &set) const; // subset
    bool        operator ==  (const Set &set) const; // equality

    bool        insert(const type item);
    bool        remove(const type item);

    long        size(); // returns the number of elements in the set.

    void        clear();

	//call quick sort libary
	void		sort();

	//hand written quick sort functions
	void		QuickSort(int p, int r);
	int			Partition(int p, int r);
	void		Swap(type &e1, type &e2);

    Set operator +   (const Set &set) const;  // union
    Set operator &&  (const Set &set) const;  // intersection
    Set operator -   (const Set &set) const;  // difference

    friend ostream & operator << (ostream &out, const Set &set);

//  private:
	  
	  type setArray[MAX_SIZE];  // array implementation
	  long position;              // point to the head element

};
