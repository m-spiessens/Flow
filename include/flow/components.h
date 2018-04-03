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

#ifndef FLOW_COMPONENTS_H_
#define FLOW_COMPONENTS_H_

#include "flow.h"

/**
 * \brief A component that inverts a value.
 *
 * The '!' operator is used to apply the inversion.
 */
template<typename Type>
class Invert: public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<Type> out;

	void run() final override
	{
		Type b;
		if (in.receive(b))
		{
			out.send(!b);
		}
	}
};

/**
 * \brief Convert between types.
 *
 * A static_cast is used to perform the conversion.
 */
template<typename From, typename To>
class Convert: public Flow::Component
{
public:
	Flow::InPort<From> inFrom;
	Flow::OutPort<To> outTo;

	void run() final override
	{
		From from;
		if (inFrom.receive(from))
		{
			outTo.send(static_cast<To>(from));
		}
	}
};

/**
 * \brief Count how many values were received.
 *
 * The counter will count from 0 to range - 1.
 * When the counter is at range - 1 and another value is received it wraps around to 0.
 */
template<typename Type>
class Counter: public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<uint32_t> out;

	/**
	 * \brief Create a counter.
	 *
	 * \param range The range specification of the counter.
	 */
	explicit Counter(uint32_t range) :
			range(range)
	{
	}

	void run() final override
	{
		Type b;
		bool more = false;
		while (in.receive(b))
		{
			counter++;
			if (counter == range)
			{
				counter = 0;
			}
			more = true;
		}
		if (more)
		{
			out.send(counter);
		}
	}

private:
	uint_fast32_t counter = 0;
	const uint_fast32_t range;
};

/**
 * \brief Count up to the upper limit then count down to the lower limit and repeat.
 */
template<typename Type>
class UpDownCounter: public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<uint32_t> out;

	explicit UpDownCounter(uint32_t downLimit, uint32_t upLimit,
			uint32_t startValue) :
			counter(startValue), upLimit(upLimit), downLimit(downLimit)
	{
	}

	void run() final override
	{
		Type b;
		bool more = false;
		while (in.receive(b))
		{
			if (up)
			{
				counter++;
			}
			else
			{
				counter--;
			}

			if (counter == upLimit)
			{
				up = false;
			}
			else if (counter == downLimit)
			{
				up = true;
			}

			more = true;
		}

		if (more)
		{
			out.send(counter);
		}
	}

private:
	uint_fast32_t counter;
	const uint_fast32_t upLimit;
	const uint_fast32_t downLimit;
	bool up = true;
};

/**
 * Provides one-to-many semantic.
 */
template<typename Type, uint8_t outputs>
class Split: public Flow::Component
{
public:
	Flow::InPort<Type> in;
	Flow::OutPort<Type> out[outputs];

	void run() final override
	{
		Type b;
		if (in.receive(b))
		{
			for (uint_fast8_t i = 0; i < outputs; i++)
			{
				out[i].send(b);
			}
		}
	}
};

/**
 * Provides many-to-one semantic.
 *
 * The input port with lower index is given priority.
 * All input ports are handled in depth-first semantic:
 * all values of a input port will be processed before going to the next input port.
 */
template<typename Type, uint8_t inputs>
class Combine: public Flow::Component
{
public:
	Flow::InPort<Type> in[inputs];
	Flow::OutPort<Type> out;

	void run() final override
	{
		for (uint_fast8_t i = 0; i < inputs; i++)
		{
			Type b;
			while (in[i].receive(b))
			{
				out.send(b);
			}
		}
	}
};

typedef char Tick;
#define TICK ((Tick)0)

/**
 * \brief Give an indication every period.
 *
 * This component can live in interrupt context of
 * a "systick" timer as an alternative to a regular software timer.
 */
class Timer: public Flow::Component
{
public:
	Flow::OutPort<Tick> outTick;

	explicit Timer(uint32_t period);

	void run() final override;

private:
	const uint_fast32_t period;
	uint_fast32_t sysTicks = 0;
};

/**
 * \brief Toggles every indication (tick).
 */
class Toggle: public Flow::Component
{
public:
	Flow::InPort<Tick> tick;
	Flow::OutPort<bool> out;

	void run() final override;

private:
	bool toggle = false;
};

#endif /* FLOW_COMPONENTS_H_ */
