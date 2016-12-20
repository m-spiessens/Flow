/*
The MIT License (MIT)

Copyright (c) 2016 Cynara Krewe

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

#ifndef FLOW_FLOW_H_
#define FLOW_FLOW_H_

#include "queue.h"

namespace Flow
{

template<typename Type>
class InPort;

template<typename Type>
class OutPort;

class Connection
{
public:
	virtual ~Connection(){}
};

template<typename Type>
class WithReceiver
{
public:
	WithReceiver(InPort<Type>& receiver)
	:	receiver(receiver)
	{
		receiver.connect(this);
	}
	virtual ~WithReceiver()
	{
		receiver.disconnect();
	}
	virtual bool receive(Type& element) = 0;
	virtual bool peek() = 0;
private:
	InPort<Type>& receiver;
};

template<typename Type>
class WithSender
{
public:
	WithSender(OutPort<Type>& sender)
	:	sender(sender)
	{
		sender.connect(this);
	}
	virtual ~WithSender()
	{
		sender.disconnect();
	}
	virtual bool send(const Type& element) = 0;
	virtual bool full() = 0;
private:
	OutPort<Type>& sender;
};

template<typename Type>
class ConnectionFIFO
:	public Connection,
	public WithSender<Type>,
	public WithReceiver<Type>,
	protected Queue<Type>
{
public:
	ConnectionFIFO(OutPort<Type>& sender, InPort<Type>& receiver, unsigned int size)
	:	WithSender<Type>(sender),
		WithReceiver<Type>(receiver),
		Queue<Type>(size)
	{
	}
	virtual ~ConnectionFIFO(){}
	bool send(const Type& element) override
	{
		return this->enqueue(element);
	}
	bool receive(Type& element) override
	{
		return this->dequeue(element);
	}
	bool peek() override
	{
		return !this->isEmpty();
	}
	bool full() override
	{
		return this->isFull();
	}
};

template<typename Type>
class ConnectionConstant
:	public WithReceiver<Type>,
	public Connection
{
public:
	ConnectionConstant(Type constant, InPort<Type>& receiver)
	:	WithReceiver<Type>(receiver),
		constant(constant)
	{
	}
	virtual ~ConnectionConstant(){}
	bool receive(Type& element) override
	{
		element = constant;
		return true;
	}
	bool peek() override
	{
		return true;
	}
private:
	const Type constant;
};

template<typename Type>
class InPort
{
public:
	InPort<Type>()
	:	connection(nullptr)
	{}
	bool receive(Type& element)
	{
		return this->isConnected() ? this->connection->receive(element) : false;
	}
	bool peek()
	{
		return this->isConnected() ? this->connection->peek() : false;
	}
private:
	WithReceiver<Type>* connection = nullptr;

	void connect(WithReceiver<Type>* connection)
	{
		this->connection = connection;
	}

	void disconnect()
	{
		this->connection = nullptr;
	}

	bool isConnected()
	{
		return this->connection != nullptr;
	}

	friend class WithReceiver<Type>;
};

template<typename Type>
class OutPort
{
public:
	OutPort<Type>()
	:	connection(nullptr)
	{}
	bool send(const Type& element)
	{
		return this->isConnected() ? this->connection->send(element) : false;
	}
	bool full()
	{
		return this->isConnected() ? this->connection->full() : false;
	}
private:
	WithSender<Type>* connection;

	void connect(WithSender<Type>* connection)
	{
		this->connection = connection;
	}

	void disconnect()
	{
		this->connection = nullptr;
	}

	bool isConnected()
	{
		return this->connection != nullptr;
	}

	friend class WithSender<Type>;
};

template<typename Type>
Connection* connect(OutPort<Type>& sender, InPort<Type>& receiver, unsigned int size = 1)
{
	return new ConnectionFIFO<Type>(sender, receiver, size);
}

template<typename Type>
Connection* connect(Type constant, InPort<Type>& receiver)
{
	return new ConnectionConstant<Type>(constant, receiver);
}

class Component
{
public:
	virtual ~Component(){}
	virtual void run() = 0;
};

} //namespace Flow


#endif /* FLOW_FLOW_H_ */
