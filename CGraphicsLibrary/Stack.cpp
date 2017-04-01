#ifdef _STACK_

template <class T>
Stack<T>::Stack()
{
	// Empty
}

template <class T>
Stack<T>::~Stack()
{
	// Empty
}

template <class T>
void Stack<T>::push(T item)
{
	_stack.append(item);
}

template <class T>
T Stack<T>::pop()
{
	return _stack.pop_back();
}

template <class T>
T Stack<T>::top()
{
	return _stack[_stack.length() - 1];
}

template <class T>
void Stack<T>::clear()
{
	_stack.clear();
}

template <class T>
int Stack<T>::size()
{
	return _stack.length();
}

template <class T>
bool Stack<T>::is_empty()
{
	return !_stack.length();
}

#endif