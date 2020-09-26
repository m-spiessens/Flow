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

#ifndef FLOW_FLOW_H_
#define FLOW_FLOW_H_

#include <assert.h>
#include <signal.h>

#include "queue.h"

/**
 * \brief Flow is a pipes and filters implementation tailored for (but not exclusive to) microcontrollers.
 */
namespace Flow
{

template<typename Type>
class InPort;

template<typename Type>
class OutPort;

template<typename Type>
class InOutPort;

class Component;

class Reactor;

/**
 * \brief A connection between component ports.
 *
 * \note Recommendation: use Flow::disconnect() instead.
 */
class Connection
{
public:
	virtual ~Connection()
	{
	}
};

template<typename Type>
class ConnectionOfType :
		public Connection
{
public:
	/**
	 * \brief Send an element over the connection.
	 *
	 * If the buffering capacity of the connection is full the given element is not added.
	 *
	 * \param element The element to be sent.
	 * \return The element was successfully sent.
	 */
	virtual bool send(const Type& element) = 0;

	/**
	 * \brief Receive an element from the connection.
	 *
	 * \param element [output] The received element.
	 * 		The return value indicates whether the element is valid.
	 * \return An element was successfully received.
	 * 		Thus the element output parameter has a valid value.
	 */
	virtual bool receive(Type& element) = 0;

	/**
	 * \brief Is an element available for receiving?
	 */
	virtual bool peek() const = 0;

	/**
	 * \brief Is the connection full?
	 */
	virtual bool full() const = 0;
};

/**
 * \brief A representation of a component.
 */
class Component
{
public:
	Component();

	virtual ~Component()
	{
	}

	/**
	 * \brief Request the component to be run by the Flow::Reactor.
	 *
	 * This is part of the Flow::Reactor internal infrastructure.
	 *
	 * \remark DO NOT manually call this unless you know what you are doing.
	 */
	void request();

    /**
     * \brief Perform second stage initialization of the component.
     *
     * Initialization which cannot be implemented in the constructor (timing wise)
     * should be implemented here.
     *
     * \remark DO NOT manually call this function if you are using the Flow::Reactor.
     * Calling Flow::Reactor::start() will execute this for all Flow::Component
     * of the application.
     */
    virtual void start(){}

    /**
     * \brief Symmetrical deinitialization, see start().
     *
     * \remark DO NOT manually call this function if you are using the Flow::Reactor.
     * Calling Flow::Reactor::stop() will execute this for all Flow::Component
     * of the application.
     */
    virtual void stop(){}

    /**
     * \brief Let the component execute its functionality.
     *
     * Typically the component will receive data from its input port(s),
     * perform a specific function and send the result(s) to its output port(s).
     *
     * \remark DO NOT manually call this function if you are using the Flow::Reactor,
     * it will do that for you when needed.
     */
    virtual void run() = 0;

private:
	volatile sig_atomic_t _request = 0;
	volatile sig_atomic_t _execute = 0;
	Component* next = nullptr;

	/**
	 * \brief Check if a request to run this component was made. If so, run the component.
	 *
	 * \return Whether the component was run or not.
	 */
	bool tryRun();

	friend class Reactor;
};

/**
 * \brief A connection of some type between component ports.
 *
 * \note Recommendation: use Flow::connect() instead.
 */
template<typename Type>
class ConnectionFIFO :
		public ConnectionOfType<Type>,
		protected Queue<Type>
{
public:
	/**
	 * \brief Create a connection between an output and input port.
	 *
	 * \param sender The output port to be connected.
	 * \param receiver The input port to be connected.
	 * \param size The amount of elements the connection can buffer.
	 */
	ConnectionFIFO(OutPort<Type>& sender, InPort<Type>& receiver,
			uint16_t size) :
			Queue<Type>(size), receiver(receiver), sender(sender)
	{
		receiver.connect(this);
		sender.connect(this);
	}

	/**
	 * \brief Destructor.
	 */
	virtual ~ConnectionFIFO()
	{
		sender.disconnect();
		receiver.disconnect();
	}

	/**
	 * \brief Send an element over the connection.
	 *
	 * Can be called concurrently with respect to receive().
	 * If the buffering capacity of the connection is full the given element is not added.
	 *
	 * \param element The element to be sent.
	 * \return The element was successfully sent.
	 */
	bool send(const Type& element) final override
	{
		receiver.request();
		return this->enqueue(element);
	}

	/**
	 * \brief Receive an element from the connection.
	 *
	 * Can be called concurrently with respect to send().
	 *
	 * \param element [output] The received element.
	 * 		The return value indicates whether the element is valid.
	 * \return An element was successfully received.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool receive(Type& element) final override
	{
		return this->dequeue(element);
	}

	/**
	 * \brief Is an element available for receiving?
	 */
	bool peek() const final override
	{
		return !this->isEmpty();
	}

	/**
	 * \brief Is the connection full?
	 */
	bool full() const final override
	{
		return this->isFull();
	}

private:
	InPort<Type>& receiver;
	OutPort<Type>& sender;
};

/**
 * \brief A bidirectional connection of some type between bidirectional component ports.
 *
 * \note Recommendation: use Flow::connect() instead.
 */
template<typename Type>
class BiDirectionalConnectionFIFO :
		public Connection
{
public:
	BiDirectionalConnectionFIFO(InOutPort<Type>& portA, InOutPort<Type>& portB,
			uint16_t size) :
			connectionA(ConnectionFIFO<Type>(portA, portB, size)),
			connectionB(ConnectionFIFO<Type>(portB, portA, size))
	{}

private:
	ConnectionFIFO<Type> connectionA, connectionB;
};

/**
 * \brief An input port of a component.
 */
template<typename Type>
class InPort
{
public:
	/**
	 * \brief Create an input port.
	 */
	explicit InPort<Type>(Component* owner) :
			owner(owner),
			connection(nullptr)
	{
	}

	/**
	 * \brief Receive an element from the input port.
	 *
	 * Can be called concurrently with respect to send() of the connected output port.
	 *
	 * \param element [output] The received element.
	 * 		The return value indicates whether the element is valid.
	 * \return An element was successfully received.
	 * 		Thus the element output parameter has a valid value.
	 */
	bool receive(Type& element)
	{
		return this->isConnected() ? this->connection->receive(element) : false;
	}

	/**
	 * \brief Is an element available for receiving?
	 */
	bool peek()
	{
		return this->isConnected() ? this->connection->peek() : false;
	}

	/**
	 * \brief Associate this input port with a connection.
	 *
	 * \note Recommendation: use Flow::connect() instead.
	 *
	 * \param connection The connection to be associated.
	 */
	void connect(ConnectionOfType<Type>* connection)
	{
		assert(!isConnected());
		this->connection = connection;
	}

	/**
	 * \brief Dissociate this input port and it's connection.
	 *
	 * \note Recommendation: use Flow::disconnect() instead.
	 */
	void disconnect()
	{
		this->connection = nullptr;
	}

	/**
	 * \brief DO NOT USE.
	 *
	 * Used by the Flow::Reactor, it should not be used manually.
	 */
	void request()
	{
		if(owner != nullptr)
		{
			owner->request();
		}
	}

	/**
	 * \brief Check if the connection is full.
	 *
	 * \return True when the connection is full.
	 * False when the connection is not full or the port is not connected.
	 */
	bool full() const
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

private:
	Component* owner = nullptr;
	ConnectionOfType<Type>* connection = nullptr;

	/**
	 * \brief Is this input port associated with a connection?
	 */
	bool isConnected() const
	{
		return this->connection != nullptr;
	}
};

/**
 * \brief An output port of a component.
 */
template<typename Type>
class OutPort
{
public:
	/**
	 * \brief Create an output port.
	 */
	OutPort<Type>() :
			connection(nullptr)
	{
	}

	/**
	 * \brief Send an element from the output port.
	 *
	 * Can be called concurrently with respect to receive() of the connected input port.
	 * If the buffering capacity of the connection is full or the port is not connected
	 * the given element is not added.
	 *
	 * \param element The element to be sent.
	 * \return The element was successfully sent.
	 */
	bool send(const Type& element)
	{
		return this->isConnected() ? this->connection->send(element) : false;
	}

	/**
	 * \brief Is the connection associated with this output port full?
	 */
	bool full()
	{
		return this->isConnected() ? this->connection->full() : false;
	}

	/**
	 * \brief Associate this output port with a connection.
	 *
	 * \note Recommendation: use Flow::connect() instead.
	 *
	 * \param connection The connection to be associated.
	 */
	void connect(ConnectionOfType<Type>* connection)
	{
		assert(!isConnected());
		this->connection = connection;
	}

	/**
	 * \brief Dissociate this output port and it's connection.
	 *
	 * \note Recommendation: use Flow::disconnect() instead.
	 */
	void disconnect()
	{
		this->connection = nullptr;
	}

private:
	ConnectionOfType<Type>* connection;

	bool isConnected() const
	{
		return this->connection != nullptr;
	}
};

/**
 * \brief A bidirectional port of a component.
 */
template<typename Type>
class InOutPort
:	public InPort<Type>,
	public OutPort<Type>
{
public:
	/**
	 * \brief Create an in-out port.
	 */
	explicit InOutPort<Type>(Component* owner)
	:	InPort<Type>(owner),
		OutPort<Type>()
	{}
};

/**
 * \brief Remove a connection.
 *
 * \param connection The connection to be removed.
 */
void disconnect(Connection* connection);

/**
 * \brief Connect an output port to an input port.
 *
 * \param sender The output port to be connected.
 * \param receiver The input port to be connected.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(OutPort<Type>& sender, InPort<Type>& receiver,
		uint16_t size = 1)
{
	return new ConnectionFIFO<Type>(sender, receiver, size);
}

/**
 * \brief Connect an output port to an input port.
 *
 * \param sender The output port to be connected.
 * \param receiver The input port to be connected.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(OutPort<Type>* sender, InPort<Type>& receiver,
		uint16_t size = 1)
{
	assert(sender != nullptr);

	return new ConnectionFIFO<Type>(*sender, receiver, size);
}

/**
 * \brief Connect an output port to an input port.
 *
 * \param sender The output port to be connected.
 * \param receiver The input port to be connected.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(OutPort<Type>& sender, InPort<Type>* receiver,
		uint16_t size = 1)
{
	assert(receiver != nullptr);

	return new ConnectionFIFO<Type>(sender, *receiver, size);
}

/**
 * \brief Connect an output port to an input port.
 *
 * \param sender The output port to be connected.
 * \param receiver The input port to be connected.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(OutPort<Type>* sender, InPort<Type>* receiver,
		uint16_t size = 1)
{
	assert(sender != nullptr);
	assert(receiver != nullptr);

	return new ConnectionFIFO<Type>(*sender, *receiver, size);
}

/**
 * \brief Connect two bidirectional ports.
 *
 * \param portA One of the bidirectional ports.
 * \param portB The other of the bidirectional ports.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(InOutPort<Type>& portA, InOutPort<Type>& portB,
		uint16_t size = 1)
{
	return new BiDirectionalConnectionFIFO<Type>(portA, portB, size);
}

/**
 * \brief Connect two bidirectional ports.
 *
 * \param portA One of the bidirectional ports.
 * \param portB The other of the bidirectional ports.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(InOutPort<Type>* portA, InOutPort<Type>& portB,
		uint16_t size = 1)
{
	assert(portA != nullptr);

	return new BiDirectionalConnectionFIFO<Type>(*portA, portB, size);
}

/**
 * \brief Connect two bidirectional ports.
 *
 * \param portA One of the bidirectional ports.
 * \param portB The other of the bidirectional ports.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(InOutPort<Type>& portA, InOutPort<Type>* portB,
		uint16_t size = 1)
{
	assert(portB != nullptr);

	return new BiDirectionalConnectionFIFO<Type>(portA, *portB, size);
}

/**
 * \brief Connect two bidirectional ports.
 *
 * \param portA One of the bidirectional ports.
 * \param portB The other of the bidirectional ports.
 * \param size The amount of elements the connection can buffer.
 */
template<typename Type>
Connection* connect(InOutPort<Type>* portA, InOutPort<Type>* portB,
		uint16_t size = 1)
{
	assert(portA != nullptr);
	assert(portB != nullptr);

	return new BiDirectionalConnectionFIFO<Type>(*portA, *portB, size);
}

} //namespace Flow

#endif /* FLOW_FLOW_H_ */
