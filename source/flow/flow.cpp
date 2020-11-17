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

#include "flow/flow.h"
#include "flow/platform.h"
#include "flow/reactor.h"
#include "flow/utility.h"

namespace Flow {

void disconnect(Connection* connection)
{
	delete connection;
}

Component::Component()
{
	Reactor::add(*this);
}

bool Component::tryRun()
{
	bool doRun = false;

	Peek* peekable = this->peekable;
	while(!doRun && peekable != nullptr)
	{
		doRun = peekable->peek();
		peekable = peekable->next;
	}

	if(doRun)
	{
		run();
	}

	return doRun;
}

Peek::Peek(Component* owner)
{
	if(owner != nullptr)
	{
		if(owner->peekable == nullptr)
		{
			owner->peekable = this;
		}
		else
		{
			Peek* tail = owner->peekable;
			while(tail->next != nullptr)
			{
				tail = tail->next;
			}
			tail->next = this;
		}
	}
}

ConnectionTrigger::ConnectionTrigger(OutTrigger& sender, InTrigger& receiver) :
	sender(sender), receiver(receiver)
{
	sender.connect(this);
	receiver.connect(this);
}

ConnectionTrigger::~ConnectionTrigger()
{
	sender.disconnect();
	receiver.disconnect();
}

bool ConnectionTrigger::send()
{
	bool available = !full();

	if(available)
	{
		_send++;
	}

	return available;
}

bool ConnectionTrigger::receive()
{
	bool available = peek();

	if(available)
	{
		_receive++;
	}

	return available;
}

bool ConnectionTrigger::peek() const
{
	return _send != _receive;
}

bool ConnectionTrigger::full() const
{
	return _send == static_cast<uint16_t>(_receive + UINT16_MAX);
}

InTrigger::InTrigger(Component* owner) :
		owner(owner)
{
}

bool InTrigger::receive()
{
	return this->isConnected() ? this->connection->receive() : false;
}

bool InTrigger::peek()
{
	return this->isConnected() ? this->connection->peek() : false;
}

void InTrigger::connect(ConnectionTrigger* connection)
{
	assert(!isConnected());
	this->connection = connection;
}

void InTrigger::disconnect()
{
	this->connection = nullptr;
}

bool InTrigger::full() const
{
	if(connection != nullptr)
	{
		return connection->full();
	}
	else
	{
		return false;
	}
}

bool InTrigger::isConnected() const
{
	return this->connection != nullptr;
}

bool OutTrigger::send()
{
	return this->isConnected() ? this->connection->send() : false;
}

bool OutTrigger::full()
{
	return this->isConnected() ? this->connection->full() : false;
}

void OutTrigger::connect(ConnectionTrigger* connection)
{
	assert(!isConnected());
	this->connection = connection;
}

void OutTrigger::disconnect()
{
	this->connection = nullptr;
}

bool OutTrigger::isConnected() const
{
	return this->connection != nullptr;
}

Connection* connect(OutTrigger& sender, InTrigger& receiver)
{
	return new ConnectionTrigger(sender, receiver);
}

Connection* connect(OutTrigger* sender, InTrigger& receiver)
{
	assert(sender != nullptr);

	return new ConnectionTrigger(*sender, receiver);
}

Connection* connect(OutTrigger& sender, InTrigger* receiver)
{
	assert(receiver != nullptr);

	return new ConnectionTrigger(sender, *receiver);
}

Connection* connect(OutTrigger* sender, InTrigger* receiver)
{
	assert(sender != nullptr);
	assert(receiver != nullptr);

	return new ConnectionTrigger(*sender, *receiver);
}

} // namespace Flow
