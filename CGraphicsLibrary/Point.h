#pragma once


struct Point {
	int x;
	int y;
	unsigned int color;

	Point() : x(0), y(0xff), color(0xffffffff) { }
	Point(int xCoord, int yCoord) : color(0xffffffff)
	{
		x = xCoord;
		y = yCoord;
	}

	Point(int xCoord, int yCoord, unsigned int color)
	{
		x = xCoord;
		y = yCoord;
		this->color = color;
	}

	Point operator-(const Point& p2) const
	{
		return Point(x - p2.x, y - p2.y);
	}
};


enum class Algorithm {
	DDA = 0,
	BRESENHAM,
	ALTER,
	ANTIALIASED
};


enum class Octant {
	ONE = 0,
	TWO,
	FOUR,
	THREE,
	EIGHT,
	SEVEN,
	FIVE,
	SIX
};


/*  Point Array -------------------------------------------------------------------- */


class PointArray
{
public:
	PointArray();
	PointArray(const PointArray& p_array);
	~PointArray();

	void append(Point point);
	Point pop_back();
	void clear();
	int length();
	void swapIndices(int x, int y);
	void print();
	void pointArrayTest();

	Point operator[](int index)
	{
		return _array[index];
	}

private:
	int _lengthAllocated;
	int _lengthTotal;
	Point* _array;
};