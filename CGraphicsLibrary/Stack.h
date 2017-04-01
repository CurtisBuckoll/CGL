#ifndef _STACK_
#define _STACK_

#include "DynamicArray.h"

template <class T>
class Stack
{
public:
	Stack();
	~Stack();
	void push(T item);
	T pop();
	T top();
	void clear();
	int size();
	bool is_empty();

private:
	DynamicArray<T> _stack;
};

#include "Stack.cpp"
#endif
