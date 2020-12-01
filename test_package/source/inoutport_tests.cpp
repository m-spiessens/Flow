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
using Flow::InOutPort;
using Flow::connect;

const static unsigned int CONNECTION_FIFO_SIZE = 10;

TEST_GROUP(InOutPort_TestBench)
{
	InOutPort<Data> unitUnderTestA{ nullptr };
	InOutPort<Data> unitUnderTestB{ nullptr };
	Connection* connectionAB;

	InOutPort<Data>* unitUnderTestC;
	InOutPort<Data> unitUnderTestD{ nullptr };
	Connection* connectionCD;

	InOutPort<Data> unitUnderTestE{ nullptr };
	InOutPort<Data>* unitUnderTestF;
	Connection* connectionEF;

	InOutPort<Data>* unitUnderTestG;
	InOutPort<Data>* unitUnderTestH;
	Connection* connectionGH;

	void setup()
	{
		unitUnderTestC = new InOutPort<Data>{ nullptr };
		unitUnderTestF = new InOutPort<Data>{ nullptr };
		unitUnderTestG = new InOutPort<Data>{ nullptr };
		unitUnderTestH = new InOutPort<Data>{ nullptr };

		connectionAB = connect(unitUnderTestA, unitUnderTestB,
				CONNECTION_FIFO_SIZE);

		connectionCD = connect(unitUnderTestC, unitUnderTestD,
				CONNECTION_FIFO_SIZE);

		connectionEF = connect(unitUnderTestE, unitUnderTestF,
				CONNECTION_FIFO_SIZE);

		connectionGH = connect(unitUnderTestG, unitUnderTestH,
				CONNECTION_FIFO_SIZE);

		delete unitUnderTestC;
		delete unitUnderTestF;
		delete unitUnderTestG;
		delete unitUnderTestH;
	}

	void teardown()
	{
		disconnect(connectionAB);
		disconnect(connectionCD);
		disconnect(connectionEF);
		disconnect(connectionGH);
	}
};

TEST(InOutPort_TestBench, IsEmptyAfterCreation)
{
	CHECK(!unitUnderTestA.peek());
	Data response;
	CHECK(!unitUnderTestA.receive(response));

	CHECK(!unitUnderTestB.peek());
	CHECK(!unitUnderTestB.receive(response));
}

TEST(InOutPort_TestBench, SendReceiveItem)
{
	CHECK(!unitUnderTestA.peek());
	Data stimulus = Data(123, true);
	CHECK(unitUnderTestB.send(stimulus));
	CHECK(unitUnderTestA.peek());
	Data response;
	CHECK(unitUnderTestA.receive(response));
	CHECK(stimulus == response);
	CHECK(!unitUnderTestA.peek());
	CHECK(!unitUnderTestA.receive(response));

	CHECK(!unitUnderTestB.peek());
	CHECK(unitUnderTestA.send(stimulus));
	CHECK(unitUnderTestB.peek());
	CHECK(unitUnderTestB.receive(response));
	CHECK(stimulus == response);
	CHECK(!unitUnderTestB.peek());
	CHECK(!unitUnderTestB.receive(response));
}

static void producer(InOutPort<Data>* _unitUnderTest,
		const unsigned long long count)
{
	for (unsigned long long c = 0; c <= count; c++)
	{
		while (!_unitUnderTest->send(Data(c, ((c % 2) == 0))))
			;
	}
}

static void consumer(InOutPort<Data>* _unitUnderTest,
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

TEST(InOutPort_TestBench, Threadsafe)
{
	const unsigned long long count = 1000000;
	bool success = true;

	std::thread producerAThread(producer, &unitUnderTestA, count);
	std::thread consumerBThread(consumer, &unitUnderTestB, count, &success);

	producerAThread.join();
	consumerBThread.join();

	CHECK(success);

	success = true;

	std::thread producerBThread(producer, &unitUnderTestB, count);
	std::thread consumerAThread(consumer, &unitUnderTestA, count, &success);

	producerBThread.join();
	consumerAThread.join();

	CHECK(success);
}
