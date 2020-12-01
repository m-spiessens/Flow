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
#include <vector>

#include "CppUTest/TestHarness.h"
#include "CppUTestExt/MockSupport.h"

#include "flow/components.h"
#include "flow/reactor.h"
#include "flow/utility.h"

#include "data.h"

using Flow::Test::Reactor;

TEST_GROUP(TestReactor_TestBench)
{
	Reactor* reactor;

	SoftwareTimer* timer;
	Counter<Tick>* counterA;
	Counter<uint32_t>* counterB;

	std::vector<Flow::Connection*> connections;

	Flow::InPort<uint32_t> inCount{ nullptr };

	void setup()
	{
		reactor = new Reactor;

		timer = new SoftwareTimer{1};
		counterA = new Counter<Tick>{UINT32_MAX};
		counterB = new Counter<uint32_t>{UINT32_MAX};

		connections =
		{
			Flow::connect(timer->outTick, counterA->in),
			Flow::connect(counterA->out, counterB->in),
			Flow::connect(counterB->out, inCount)
		};
	}

	void teardown()
	{
		mock().clear();

		for(Flow::Connection* connection : connections)
		{
			Flow::disconnect(connection);
		}
		connections.clear();

		delete timer;
		delete counterA;
		delete counterB;

		delete reactor;
	}
};

TEST(TestReactor_TestBench, React)
{
	reactor->start();

	uint32_t finalCount = 0;
	do
	{
		timer->isr();

		mock().expectOneCall("Platform::waitForEvent()");
		reactor->run();

		inCount.receive(finalCount);
	} while(finalCount < 100);

	mock().expectOneCall("Platform::waitForEvent()");
	reactor->run();

	reactor->stop();

	mock().checkExpectations();
}
