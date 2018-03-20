/*
 The MIT License (MIT)

 Copyright (c) 2017 Cynara Krewe

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

#include <stdint.h>
#include <thread>

#include "CppUTest/TestHarness.h"

#include "flow/flow.h"

#include "flow_test/data.h"

using Flow::Connection;
using Flow::OutPort;
using Flow::InPort;
using Flow::connect;

const static unsigned int CONNECTION_FIFO_SIZE = 10;

TEST_GROUP(Port_TestBench)
{
	Connection* connection;
	OutPort<Data> outUnitUnderTest;
	InPort<Data> inUnitUnderTest;

	void setup()
	{
		connection = connect(outUnitUnderTest, inUnitUnderTest,
				CONNECTION_FIFO_SIZE);
	}

	void teardown()
	{
		disconnect(connection);
	}
};

TEST(Port_TestBench, IsEmptyAfterCreation)
{
	CHECK(!inUnitUnderTest.peek());
	Data response;
	CHECK(!inUnitUnderTest.receive(response));
	CHECK(!outUnitUnderTest.full());
}

TEST(Port_TestBench, SendReceiveItem)
{
	CHECK(!inUnitUnderTest.peek());
	Data stimulus = Data(123, true);
	CHECK(outUnitUnderTest.send(stimulus));
	CHECK(inUnitUnderTest.peek());
	Data response;
	CHECK(inUnitUnderTest.receive(response));
	CHECK(stimulus == response);
	CHECK(!inUnitUnderTest.peek());
	CHECK(!inUnitUnderTest.receive(response));
}

TEST(Port_TestBench, FullConnection)
{
	// Port should be empty.
	CHECK(!inUnitUnderTest.peek());

	for (unsigned int c = 0; c < (CONNECTION_FIFO_SIZE - 1); c++)
	{
		Data stimulus = Data(c, true);

		// Port should accept another item.
		CHECK(outUnitUnderTest.send(stimulus));

		// Port should not be empty.
		CHECK(inUnitUnderTest.peek());
	}

	Data lastStimulus = Data(CONNECTION_FIFO_SIZE, false);
	// Port should accept another item.
	CHECK(outUnitUnderTest.send(lastStimulus));

	// Port should not be empty.
	CHECK(inUnitUnderTest.peek());
	CHECK(outUnitUnderTest.full());

	// Port shouldn't accept any more items.
	CHECK(!outUnitUnderTest.send(lastStimulus));

	Data response;

	for (unsigned int c = 0; c < (CONNECTION_FIFO_SIZE - 1); c++)
	{
		// Should get another item from the Port.
		CHECK(inUnitUnderTest.receive(response));

		// Item should be the expected.
		Data expectedResponse = Data(c, true);
		CHECK(response == expectedResponse);

		// Port should not be empty.
		CHECK(inUnitUnderTest.peek());
		CHECK(!outUnitUnderTest.full());
	}

	// Should get another item from the Port.
	CHECK(inUnitUnderTest.receive(response));

	// Item should be the expected.
	CHECK(lastStimulus == response);

	// Port should be empty.
	CHECK(!inUnitUnderTest.peek());
	CHECK(!outUnitUnderTest.full());

	// Shouldn't get another item from the Port.
	CHECK(!inUnitUnderTest.receive(response));
}

static void producer(OutPort<Data>* _unitUnderTest,
		const unsigned long long count)
{
	for (unsigned long long c = 0; c <= count; c++)
	{
		while (!_unitUnderTest->send(Data(c, ((c % 2) == 0))))
			;
	}
}

static void consumer(InPort<Data>* _unitUnderTest,
		const unsigned long long count, bool* success)
{
	unsigned long long c = 0;

	while (c <= count)
	{
		Data response;
		if (_unitUnderTest->receive(response))
		{
			Data expected = Data(c, ((c % 2) == 0));
			*success = *success && (response == expected);
			c++;
		}
	}
}

TEST(Port_TestBench, Threadsafe)
{
	const unsigned long long count = 1000000;
	bool success = true;

	std::thread producerThread(producer, &outUnitUnderTest, count);
	std::thread consumerThread(consumer, &inUnitUnderTest, count, &success);

	producerThread.join();
	consumerThread.join();

	CHECK(success);
}
