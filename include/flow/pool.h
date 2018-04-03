/* The MIT License (MIT)
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

/**
 * \brief Flow is a pipes and filters implementation tailored for (but not exclusive to) microcontrollers.
 */
namespace Flow
{

/**
 * \brief A pool of DataType elements.
 *
 * A pool can be used to effectively pass big data structures through connections between components.
 * An element can be taken from the pool and passed around by reference.
 * When taking an element the new owner is responsible to give it back to the pool at some point.
 *
 * A pool is thread safe in the sense that the take() and release() can be called concurrently.
 */
template<typename DataType>
class Pool
{
private:
	uint16_t _size;
	DataType* _data;
	Queue<DataType*> _available;

public:
	/**
	 * \brief Create a heap.
	 *
	 * The array of DataType will be allocated on the heap.
	 *
	 * \param size The size of the pool in number of DataType.
	 */
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

	/**
	 * \brief Copy constructor.
	 *
	 * Performs a complete, deep copy of the given pool.
	 * The array of DataType will be allocated on the heap.
	 *
	 * \param other Pool to be copied.
	 */
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

	/**
	 * \brief Assignment operator.
	 */
	Pool& operator=(const Pool<DataType>& other)
	{
		Pool<DataType> shadow(other);
		*this = std::move(shadow);
		return *this;
	}

	/**
	 * \brief Move operator.
	 */
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

	/**
	 * \brief Destructor.
	 *
	 * Deallocates the array of DataType from the heap.
	 */
	~Pool()
	{
		delete[] _data;
	}

	/**
	 * \brief Is an element available in the pool?
	 */
	bool haveAvailable() const
	{
		return !_available.isEmpty();
	}

	/**
	 * \brief Take an element from the pool.
	 *
	 * When an element is taken from the pool, the new "owner" is responsible
	 * to release it back into the pool when it is no longer needed.
	 *
	 * \return Pointer to an element if the pool had one available.
	 * 		nullptr if no element was available.
	 */
	DataType* take()
	{
		DataType* take = nullptr;

		_available.dequeue(take);

		return take;
	}

	/**
	 * \brief Release an element into the pool.
	 *
	 * \param element The element to be released into the pool.
	 * \return The element was successfully put in the pool.
	 * 		When not successful the take-release mechanism was violated.
	 */
	bool release(DataType& element)
	{
		return _available.enqueue(&element);
	}
};

} // namespace Flow

#endif /* FLOW_POOL_H_ */
