/**
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Cynara Krewe
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software, hardware and associated documentation files (the "Solution"), to deal
 * in the Solution without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Solution, and to permit persons to whom the Solution is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Solution.
 *
 * THE SOLUTION IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOLUTION OR THE USE OR OTHER DEALINGS IN THE
 * SOLUTION.
 */

#ifndef FLOW_POOL_H_
#define FLOW_POOL_H_

#include <stdint.h>

#include "queue.h"

namespace Flow
{

template<typename DataType>
class Pool
{
private:
	uint16_t _size;
	DataType* _data;
	Queue<DataType*> _available;

public:
	Pool(uint16_t size) :
			_size(size), _data(new DataType[size]), _available(
					Queue<DataType*>(_size))
	{
		for (uint_fast16_t i = 0; i < _size; i++)
		{
			_available.enqueue(&_data[i]);
		}
	}

	~Pool()
	{
		delete[] _data;
	}

	bool haveAvailable() const
	{
		return !_available.isEmpty();
	}

	DataType* take()
	{
		DataType* take = nullptr;

		_available.dequeue(take);

		return take;
	}

	bool release(DataType& element)
	{
		return _available.enqueue(&element);
	}
};

} // namespace Flow

#endif /* FLOW_POOL_H_ */