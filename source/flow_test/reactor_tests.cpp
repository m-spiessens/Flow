/* The MIT License (MIT)
 *
 * Copyright (c) 2020 Cynara Krewe
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

#include <stdint.h>
#include <thread>

#include "CppUTest/TestHarness.h"

#include "flow/components.h"
#include "flow/reactor.h"
#include "flow/utility.h"

#include "flow_test/data.h"

TEST_GROUP(Reactor_TestBench)
{
	Flow::Connection* connection[2];
	SoftwareTimer timer{1};
	Counter<Tick> counter{UINT32_MAX};

#define COUNT 1000llu

	Flow::InPort<uint32_t> inCount{&dummyComponent};

	void setup()
	{
		connection[0] = Flow::connect(timer.outTick, counter.in, COUNT);
		connection[1] = Flow::connect(counter.out, inCount, COUNT);
	}

	void teardown()
	{
		for (unsigned int i = 0; i < ArraySizeOf(connection); i++)
		{
			delete connection[i];
		}

		Flow::Reactor::reset();
	}
};

static void producer(SoftwareTimer* timer, uint64_t count)
{
	for (unsigned long long c = 0; c <= count; c++)
	{
		timer->isr();
	}
}

TEST(Reactor_TestBench, React)
{
	std::thread producerThread(producer, &timer, COUNT);

	Flow::Reactor::start();

	uint32_t finalCount = 0;
	do
	{
		Flow::Reactor::run();
		inCount.receive(finalCount);
	} while(finalCount < COUNT);

	producerThread.join();

	Flow::Reactor::stop();
}
