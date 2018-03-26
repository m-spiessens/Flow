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

#ifndef FLOW_QUEUE_H_
#define FLOW_QUEUE_H_

#include <stdint.h>
#include <utility>

namespace Flow
{

/**
 * \brief Implementation of a queue or FIFO.
 * A queue is thread safe in the sense that the enqueue() and dequeue() can be called concurrently.
 */
template<typename DataType>
class Queue
{
private:
	DataType* _data;
	uint16_t _size;
	volatile uint16_t _first;
	volatile uint16_t _last;
	volatile uint16_t _enqueued;
	volatile uint16_t _dequeued;

public:
	/**
	 * \brief Create a queue.
	 * The array of DataType will be allocated on the heap.
	 * \param size The size of the queue in number of DataType.
	 */
	explicit Queue(uint16_t size) :
			_size(size),
			_first(0),
			_last(0),
			_enqueued(0),
			_dequeued(0)
	{
		_data = new DataType[_size];
	}

	/**
	 * \brief Copy constructor.
	 * Performs a complete, deep copy of the given queue.
	 * The array of DataType will be allocated on the heap.
	 * \param other Queue to be copied.
	 */
	explicit Queue(const Queue<DataType>& other) :
			_size(other._size),
			_first(other._first),
			_last(other._last),
			_enqueued(other._enqueued),
			_dequeued(other._dequeued)
	{
		_data = new DataType[_size];

		for(uint_fast16_t i = 0; i < _size; i++)
		{
			_data[i] = other._data[i];
		}
	}

	/**
	 * \brief Assignment operator.
	 */
	Queue& operator=(const Queue<DataType>& other)
	{
		Queue<DataType> shadow(other);
		*this = std::move(shadow);
		return *this;
	}

	/**
	 * \brief Move operator.
	 */
	Queue& operator=(Queue<DataType>&& other) noexcept
	{
		if(this != &other)
		{
			delete[] _data;
			_data = other._data;
			other._data = nullptr;
			_size = other._size;
			_first = other._first;
			_last = other._last;
			_enqueued = other._enqueued;
			_dequeued = other._dequeued;
		}

		return *this;
	}

	/**
	 * \brief Destructor.
	 * Deallocates the array of DataType from the heap.
	 */
	~Queue()
	{
		delete[] _data;
	}

	/**
	 * \brief Is the queue empty?
	 */
	bool isEmpty() const
	{
		return (_enqueued == _dequeued);
	}

	/**
	 * \brief Is the queue full?
	 */
	bool isFull() const
	{
		return (_enqueued == static_cast<uint16_t>(_dequeued + _size));
	}

	/**
	 * \brief Enqueue an element of DataType.
	 * Can be called concurrently with respect to dequeue().
	 * If the queue is full the given element is not added.
	 * \param element The element to be enqueued.
	 * \return The element was successfully enqueued.
	 */
	bool enqueue(const DataType& element)
	{
		bool success = false;

		if (!isFull())
		{
			_data[_last] = element;

			_last = (_last == _size - 1) ? 0 : _last + 1;

			_enqueued++;

			success = true;
		}

		return success;
	}

	/**
	 * \brief Dequeue an element of DataType.
	 * Can be called concurrently with respect to enqueue().
	 * \param element [output] The dequeued element.
	 * 		The return value indicates whether the element is valid.
	 * \return An element was successfully dequeued.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool dequeue(DataType& element)
	{
		bool success = false;

		if (!isEmpty())
		{
			element = _data[_first];

			_first = (_first == _size - 1) ? 0 : _first + 1;

			_dequeued++;

			success = true;
		}

		return success;
	}

	/**
	 * \brief Peek in the queue.
	 * Does not modify the queue in any way.
	 * \param element [output] The next element to be dequeued.
	 * 		The return value indicates whether the element is valid.
	 * \return The queue is not empty.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool peek(DataType& element) const
	{
		bool success = false;

		if (!isEmpty())
		{
			element = _data[_first];

			success = true;
		}

		return success;
	}
};

} // namespace Flow

#endif /* FLOW_QUEUE_H_ */
