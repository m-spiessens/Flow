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

#ifndef FLOW_FLOW_H_
#define FLOW_FLOW_H_

#include "queue.h"

namespace Flow
{

template<typename Type>
class InPort;

template<typename Type>
class OutPort;

template<typename Type>
class InOutPort;

class Connection
{
public:
	virtual ~Connection()
	{
	}
};

template<typename Type>
class ConnectionOfType: public Connection,
		protected Queue<Type>
{
public:
	ConnectionOfType(OutPort<Type>& sender, InPort<Type>& receiver,
			unsigned int size) :
			Queue<Type>(size), receiver(receiver), sender(sender)
	{
		receiver.connect(this);
		sender.connect(this);
	}

	virtual ~ConnectionOfType()
	{
		sender.disconnect();
		receiver.disconnect();
	}

	bool send(const Type& element)
	{
		return this->enqueue(element);
	}

	bool receive(Type& element)
	{
		return this->dequeue(element);
	}

	bool peek()
	{
		return !this->isEmpty();
	}

	bool full()
	{
		return this->isFull();
	}

private:
	InPort<Type>& receiver;
	OutPort<Type>& sender;
};

template<typename Type>
class BiDirectionalConnectionOfType: public Connection
{
public:
	BiDirectionalConnectionOfType(InOutPort<Type>& portA, InOutPort<Type>& portB,
			unsigned int size) :
			connectionA(ConnectionOfType<Type>(portA, portB, size)),
			connectionB(ConnectionOfType<Type>(portB, portA, size))
	{}

private:
	ConnectionOfType<Type> connectionA, connectionB;
};

template<typename Type>
class InPort
{
public:
	InPort<Type>() :
			connection(nullptr)
	{
	}

	bool receive(Type& element)
	{
		return this->isConnected() ? this->connection->receive(element) : false;
	}

	bool peek()
	{
		return this->isConnected() ? this->connection->peek() : false;
	}

private:
	ConnectionOfType<Type>* connection = nullptr;

	void connect(ConnectionOfType<Type>* connection)
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

	friend class ConnectionOfType<Type> ;
};

template<typename Type>
class OutPort
{
public:
	OutPort<Type>() :
			connection(nullptr)
	{
	}

	bool send(const Type& element)
	{
		return this->isConnected() ? this->connection->send(element) : false;
	}

	bool full()
	{
		return this->isConnected() ? this->connection->full() : false;
	}

private:
	ConnectionOfType<Type>* connection;

	void connect(ConnectionOfType<Type>* connection)
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

	friend class ConnectionOfType<Type> ;
};



template<typename Type>
class InOutPort
:	public InPort<Type>,
	public OutPort<Type>
{
public:
	InOutPort<Type>()
	:	InPort<Type>(),
		OutPort<Type>()
	{}
};

template<typename Type>
Connection* connect(OutPort<Type>& sender, InPort<Type>& receiver,
		unsigned int size = 1)
{
	return new ConnectionOfType<Type>(sender, receiver, size);
}

template<typename Type>
Connection* connect(InOutPort<Type>& portA, InOutPort<Type>& portB,
		unsigned int size = 1)
{
	return new BiDirectionalConnectionOfType<Type>(portA, portB, size);
}

void disconnect(Connection* connection);

class Component
{
public:
	virtual ~Component()
	{
	}

	virtual void run() = 0;
};

} //namespace Flow

#endif /* FLOW_FLOW_H_ */
