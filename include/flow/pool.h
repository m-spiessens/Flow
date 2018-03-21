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
	explicit Pool(uint16_t size) :
			_size(size),
			_data(new DataType[_size]),
			_available(Queue<DataType*>(_size))
	{
		for (uint_fast16_t i = 0; i < _size; i++)
		{
			_available.enqueue(&_data[i]);
		}
	}

	explicit Pool(const Pool<DataType>& other) :
			_size(other._size),
			_data(new DataType[_size]),
			_available(other._available)
	{
		for (uint_fast16_t i = 0; i < _size; i++)
		{
			_data[i] = other._data[i];
		}
	}

	Pool& operator=(const Pool<DataType>& other)
	{
		Pool<DataType> shadow(other);
		*this = std::move(shadow);
		return *this;
	}

	Pool& operator=(Pool<DataType>&& other) noexcept
	{
		if(this != &other)
		{
			delete[] _data;
			_data = other._data;
			other._data = nullptr;
			_size = other._size;
			_available = other._available;
		}

		return *this;
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
