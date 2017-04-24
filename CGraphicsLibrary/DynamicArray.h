#ifndef _DYNARRAY_
#define _DYNARRAY_

#include <stdexcept>

/*  Dynamic Array -------------------------------------------------------------------- */

template <class T>
class DynamicArray
{
public:
	DynamicArray();
	DynamicArray(const DynamicArray& d_array);
	~DynamicArray();

	void append(T item);
	T pop_back();
	void clear();
    int length() const;
	void swapIndices(int x, int y);

    T operator[](int index) const
	{
		return _array[index];
	}

private:
	int _lengthAllocated;
	int _lengthTotal;
	T* _array;
};

#include "DynamicArray.cpp"
#endif
