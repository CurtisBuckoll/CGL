#include "Point.h"

#include <iostream>


/*  Class Point Array -------------------------------------------------------------- */


PointArray::PointArray()
{
	_lengthAllocated = 0;
	_lengthTotal = 3;
	_array = new Point[3];
}

PointArray::PointArray(const PointArray& p_array)
{
	_lengthAllocated = p_array._lengthAllocated;
	_lengthTotal = p_array._lengthTotal;
	_array = new Point[_lengthTotal];
	for (int i = 0; i < _lengthAllocated; i++)
	{
		_array[i] = p_array._array[i];
	}
}

PointArray::~PointArray()
{
	delete[] _array;
}

void PointArray::append(Point point)
{
	if (_lengthAllocated == _lengthTotal)
	{
		_lengthTotal = (_lengthTotal + 1) * 2;
		Point* newArr = new Point[_lengthTotal];
		for (int i = 0; i < _lengthAllocated; i++)
		{
			newArr[i] = _array[i];
		}
		delete[] _array;
		_array = newArr;
	}

	_array[_lengthAllocated] = point;
	_lengthAllocated++;
}

Point PointArray::pop_back()
{
	if (_lengthAllocated <= 0)
	{
		return Point(-1, -1);
	}
	if (_lengthAllocated < (_lengthTotal / 2) - 1)
	{
		_lengthTotal = (_lengthTotal / 2) - 1;
		Point* newArr = new Point[_lengthTotal];
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

void PointArray::clear()
{
	delete[] _array;
	_array = new Point[3];
	_lengthAllocated = 0;
	_lengthTotal = 3;
}

int PointArray::length()
{
	return _lengthAllocated;
}

void PointArray::swapIndices(int x, int y)
{
	if (x < 0 || x >= _lengthAllocated || y < 0 || y >= _lengthAllocated)
	{
		return;
	}
	Point temp;
	temp = _array[x];
	_array[x] = _array[y];
	_array[y] = temp;
}

void PointArray::print()
{
	for (int i = 0; i < _lengthAllocated; i++)
	{
		std::cout << "Index: " << i << " - (" << _array[i].x << "," << _array[i].y << "), Color: " << _array[i].color << std::endl;
	}
	std::cout << "Length Allocated: " << _lengthAllocated << ", Length Total: " << _lengthTotal << std::endl;
}

void PointArray::pointArrayTest()
{
	PointArray p_array;
	for (int i = 0; i < 20; i++)
	{
		p_array.append(Point(i, 15 - i));
	}
	for (int i = 0; i < p_array.length(); i++)
	{
		std::cout << "Index: " << i << " (" << p_array[i].x << ", " << p_array[i].y << ")" << std::endl;
	}
	p_array.print();
	std::cout << "Removing:" << std::endl;
	while( p_array.length() > 0)
	{
		Point p = p_array.pop_back();
		std::cout << "(" << p.x << ", " << p.y << ")" << " Length: " << p_array.length() << std::endl;
	}
	Point p = p_array.pop_back();
	std::cout << "(" << p.x << ", " << p.y << ")" << " Length: " << p_array.length() << std::endl;
	p_array.print();

	for (int i = 0; i < 25; i++)
	{
		p_array.append(Point(i, 15 - i));
	}
	for (int i = 0; i < p_array.length(); i++)
	{
		std::cout << "Index: " << i << " (" << p_array[i].x << ", " << p_array[i].y << ")" << std::endl;
	}
	p_array.print();
}