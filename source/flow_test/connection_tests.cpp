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

#include "flow_test/data.h"

using Flow::ConnectionFIFO;
using Flow::OutPort;
using Flow::InPort;

#define CONNECTION_FIFO_SIZE 1000

TEST_GROUP(ConnectionOfType_TestBench)
{
	ConnectionFIFO<Data>* unitUnderTest;
	OutPort<Data> sender;
	InPort<Data> receiver{ nullptr };

	void setup()
	{
		unitUnderTest = new Flow::ConnectionFIFO<Data>(sender,
				receiver, CONNECTION_FIFO_SIZE);
	}

	void teardown()
	{
		delete unitUnderTest;
	}
};

TEST(ConnectionOfType_TestBench, IsEmptyAfterCreation)
{
	CHECK(!unitUnderTest->peek());
	Data response;
	CHECK(!unitUnderTest->receive(response));
}

TEST(ConnectionOfType_TestBench, SendReceiveItem)
{
	CHECK(!unitUnderTest->peek());
	Data stimulus = Data(123, true);
	CHECK(unitUnderTest->send(stimulus));
	CHECK(unitUnderTest->peek());
	Data response;
	CHECK(unitUnderTest->receive(response));
	CHECK(stimulus == response);
	CHECK(!unitUnderTest->peek());
	CHECK(!unitUnderTest->receive(response));
}

TEST(ConnectionOfType_TestBench, FullConnection)
{
	// Connection should be empty.
	CHECK(!unitUnderTest->peek());

	for (unsigned int c = 0; c < (CONNECTION_FIFO_SIZE - 1); c++)
	{
		Data stimulus = Data(c, true);
		// Connection should accept another item.
		CHECK(unitUnderTest->send(stimulus));

		// Connection should not be empty.
		CHECK(unitUnderTest->peek());
	}

	Data lastStimulus = Data(CONNECTION_FIFO_SIZE, false);
	// Connection should accept another item.
	CHECK(unitUnderTest->send(lastStimulus));

	// Connection should not be empty.
	CHECK(unitUnderTest->peek());

	// Connection shouldn't accept any more items.
	CHECK(!unitUnderTest->send(lastStimulus));

	Data response;

	for (unsigned int c = 0; c < (CONNECTION_FIFO_SIZE - 1); c++)
	{
		// Should get another item from the Connection.
		CHECK(unitUnderTest->receive(response));

		// Item should be the expected.
		Data expectedResponse = Data(c, true);
		CHECK(response == expectedResponse);

		// Connection should not be empty.
		CHECK(unitUnderTest->peek());
	}

	// Should get another item from the Connection.
	CHECK(unitUnderTest->receive(response));

	// Item should be the expected.
	CHECK(lastStimulus == response);

	// Connection should be empty.
	CHECK(!unitUnderTest->peek());

	// Shouldn't get another item from the Connection.
	CHECK(!unitUnderTest->receive(response));
}

static void producer(ConnectionFIFO<Data>* _unitUnderTest,
		const unsigned long long count)
{
	for (unsigned long long c = 0; c <= count; c++)
	{
		while (!_unitUnderTest->send(Data(c, ((c % 2) == 0))))
			;
	}
}

static void consumer(ConnectionFIFO<Data>* _unitUnderTest,
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

TEST(ConnectionOfType_TestBench, Threadsafe)
{
	// Connection should be empty.
	CHECK(!unitUnderTest->peek());

	const unsigned long long count = 1000000;
	bool success = true;

	std::thread producerThread(producer, unitUnderTest, count);
	std::thread consumerThread(consumer, unitUnderTest, count, &success);

	producerThread.join();
	consumerThread.join();

	CHECK(success);

	// Connection should be empty.
	CHECK(!unitUnderTest->peek());
}
