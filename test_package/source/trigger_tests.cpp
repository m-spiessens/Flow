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

#include "flow/flow.h"

#include "data.h"

using Flow::Connection;
using Flow::InTrigger;
using Flow::OutTrigger;
using Flow::connect;

TEST_GROUP(Trigger_TestBench)
{
	OutTrigger unitUnderTestOut;
	InTrigger unitUnderTestIn{  nullptr  };
	Connection* connection;

	void setup()
	{
		connection = connect(unitUnderTestOut, unitUnderTestIn);
	}

	void teardown()
	{
		disconnect(connection);
	}
};

TEST(Trigger_TestBench, SendReceiveTrigger)
{
	CHECK(!unitUnderTestIn.peek());

	CHECK(unitUnderTestOut.send());

	CHECK(unitUnderTestIn.peek());
	CHECK(unitUnderTestIn.receive());

	CHECK(!unitUnderTestIn.peek());
	CHECK(!unitUnderTestIn.receive());
}

TEST(Trigger_TestBench, Connect)
{
	CHECK(unitUnderTestOut.send());
	CHECK(unitUnderTestIn.peek());
	CHECK(unitUnderTestIn.receive());

	disconnect(connection);

	CHECK_FALSE(unitUnderTestIn.full());
	CHECK(!unitUnderTestOut.send());
	CHECK(!unitUnderTestIn.peek());
	CHECK(!unitUnderTestIn.receive());

	connection = connect(&unitUnderTestOut, unitUnderTestIn);

	CHECK(unitUnderTestOut.send());
	CHECK(unitUnderTestIn.peek());
	CHECK(unitUnderTestIn.receive());

	disconnect(connection);

	CHECK_FALSE(unitUnderTestIn.full());
	CHECK(!unitUnderTestOut.send());
	CHECK(!unitUnderTestIn.peek());
	CHECK(!unitUnderTestIn.receive());

	connection = connect(unitUnderTestOut, &unitUnderTestIn);

	CHECK(unitUnderTestOut.send());
	CHECK(unitUnderTestIn.peek());
	CHECK(unitUnderTestIn.receive());

	disconnect(connection);

	CHECK_FALSE(unitUnderTestIn.full());
	CHECK(!unitUnderTestOut.send());
	CHECK(!unitUnderTestIn.peek());
	CHECK(!unitUnderTestIn.receive());

	connection = connect(&unitUnderTestOut, &unitUnderTestIn);

	CHECK(unitUnderTestOut.send());
	CHECK(unitUnderTestIn.peek());
	CHECK(unitUnderTestIn.receive());
}

TEST(Trigger_TestBench, SendReceiveTriggerFullEmptyWithOverflow)
{
	CHECK(!unitUnderTestIn.peek());

	CHECK(unitUnderTestOut.send());
	CHECK(unitUnderTestIn.receive());

	CHECK(!unitUnderTestIn.peek());

	for(int32_t i = 0; i < UINT16_MAX; i++)
	{
		CHECK(unitUnderTestOut.send());
	}

	CHECK(!unitUnderTestOut.send());
	CHECK(unitUnderTestIn.full());
	CHECK(unitUnderTestOut.full());

	for(int32_t i = 0; i < UINT16_MAX; i++)
	{
		CHECK(unitUnderTestIn.peek());
		CHECK(unitUnderTestIn.receive());
	}

	CHECK(!unitUnderTestIn.peek());
	CHECK(!unitUnderTestIn.receive());
}

static void producer(OutTrigger* _unitUnderTest,
		const unsigned long long count)
{
	for (unsigned long long c = 0; c <= count; c++)
	{
		while (!_unitUnderTest->send())
			;
	}
}

static void consumer(InTrigger* _unitUnderTest,
		const unsigned long long count, bool* success)
{
	unsigned long long c = 0;

	*success = false;

	while (c <= count)
	{
		if (_unitUnderTest->receive())
		{
			c++;
		}
	}

	*success = true;
}

TEST(Trigger_TestBench, Threadsafe)
{
	const unsigned long long count = 100000;
	bool success = true;

	std::thread producerAThread(producer, &unitUnderTestOut, count);
	std::thread consumerBThread(consumer, &unitUnderTestIn, count, &success);

	producerAThread.join();
	consumerBThread.join();

	CHECK(success);
}
