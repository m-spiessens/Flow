/*
The MIT License (MIT)

Copyright (c) 2016 Cynara Krewe

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software, hardware and associated documentation files (the "Solution"), to deal
in the Solution without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Solution, and to permit persons to whom the Solution is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Solution.

THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
SOLUTION.
 */

#ifndef FLOW_QUEUE_H_
#define FLOW_QUEUE_H_

#include <stdint.h>

namespace Flow
{

template<typename DataType>
class Queue
{
private:
	DataType* _data;
	uint16_t _size;
	uint16_t _first;
	uint16_t _last;
	volatile uint16_t _enqueued;
	volatile uint16_t _dequeued;

public:
	Queue(unsigned int size)
	:	_size(size),
		_first(0),
	 	_last(0),
	 	_enqueued(0),
	 	_dequeued(0)
	{
		_data = new DataType[size];
	}

	~Queue()
	{
		delete[] _data;
	}

	bool isEmpty() const
	{
		return (_enqueued == _dequeued);
	}

	bool isFull() const
	{
		return (_enqueued == _dequeued + _size);
	}

	bool enqueue(const DataType& element)
	{
		bool success = false;

		if(!isFull())
		{
			_data[_last] = element;

			_last = (_last == _size - 1) ? 0 : _last + 1;

			_enqueued++;
			success = true;
		}

		return success;
	}

	bool dequeue(DataType& element)
	{
		bool success = false;

		if(!isEmpty())
		{
			element = _data[_first];

			_first = (_first == _size - 1) ? 0 : _first + 1;

			_dequeued++;
			success = true;
		}

		return success;
	}

	bool peek(DataType* const element) const
	{
		bool success = false;

		if(!isEmpty())
		{
			*element = _data[_first];

			success = true;
		}

		return success;
	}
};

} // namespace Flow

#endif /* FLOW_QUEUE_H_ */
