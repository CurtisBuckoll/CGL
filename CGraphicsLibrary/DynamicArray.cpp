#ifdef _DYNARRAY_

#include <iostream>


/*  Class Point Array -------------------------------------------------------------- */

template <class T>
DynamicArray<T>::DynamicArray()
{
	_lengthAllocated = 0;
	_lengthTotal = BASE_LEN;
	_array = new T[BASE_LEN];
}

template <class T>
DynamicArray<T>::DynamicArray(const DynamicArray& d_array)
{
	_lengthAllocated = d_array._lengthAllocated;
	_lengthTotal = d_array._lengthTotal;
	_array = new T[_lengthTotal];
	for (int i = 0; i < _lengthAllocated; i++)
	{
		_array[i] = d_array._array[i];
	}
}

template <class T>
DynamicArray<T>::~DynamicArray()
{
	delete[] _array;
}

template <class T>
void DynamicArray<T>::append(T item)
{
	if (_lengthAllocated == _lengthTotal)
	{
		_lengthTotal = (_lengthTotal + 1) * 2;
		T* newArr = new T[_lengthTotal];
		for (int i = 0; i < _lengthAllocated; i++)
		{
			newArr[i] = _array[i];
		}
		delete[] _array;
		_array = newArr;
	}

	_array[_lengthAllocated] = item;
	_lengthAllocated++;
}

template <class T>
T DynamicArray<T>::pop_back()
{
	if (_lengthAllocated <= 0)
	{
		throw std::exception("DynamicArray::pop_back - array is empty");
	}
	if (_lengthAllocated < (_lengthTotal / 2) - 1)
	{
		_lengthTotal = (_lengthTotal / 2) - 1;
		T* newArr = new T[_lengthTotal];
		for (int i = 0; i < _lengthAllocated; i++)
		{
			newArr[i] = _array[i];
		}
		delete[] _array;
		_array = newArr;
	}
	_lengthAllocated--;
	return _array[_lengthAllocated];
}

template <class T>
void DynamicArray<T>::clear()
{
	//delete[] _array;
	//_array = new T[6];
	_lengthAllocated = 0;
	_lengthTotal = BASE_LEN;
}

template <class T>
int DynamicArray<T>::length() const
{
	return _lengthAllocated;
}

template <class T>
void DynamicArray<T>::swapIndices(int x, int y)
{
	if (x < 0 || x >= _lengthAllocated || y < 0 || y >= _lengthAllocated)
	{
		return;
	}
	T temp;
	temp = _array[x];
	_array[x] = _array[y];
	_array[y] = temp;
}

#endif
