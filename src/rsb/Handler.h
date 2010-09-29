/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This program is free software; you can redistribute it
 * and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation;
 * either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * ============================================================ */

#ifndef HANDLER_H_
#define HANDLER_H_

#include "RSBEvent.h"

#include <boost/shared_ptr.hpp>

namespace rsb {

class Handlers {
public:
	enum Type {
		DATA, EVENT
	};
};

/**
 * A handler is a "callback object" invoked by a @ref rsb::Subscriber when new data
 * is available that matches the @ref rsb::Subscription this handler is attached to.
 *
 * @author swrede
 */
class Handler {
public:
	Handler(Handlers::Type t) :
		type(t) {
	}
	virtual ~Handler() {
	}

	// TODO make subscription a friend to hide this method from public API
	virtual void internal_notify(RSBEventPtr) = 0;
protected:
	Handlers::Type type;
};

typedef boost::shared_ptr<Handler> HandlerPtr;

// TODO makes even more sense if RSBEvent would be a template type
class EventHandler: public Handler {
public:
	EventHandler() :
		Handler(Handlers::EVENT) {
	}
	virtual ~EventHandler() {
	}

	virtual void internal_notify(RSBEventPtr e) {
		notify(e);
	}

	virtual void notify(RSBEventPtr) = 0;

};

/**
 * A utility class to simplify data handling by automatically passing the data
 * of the desired type to the notify() method.
 *
 * @tparam T desired data type to retrieve from the subscriber
 * @author swrede
 */
template<class T>
class DataHandler: public Handler {
public:

	/**
	 * Shared pointer of the target type.
	 */
	typedef boost::shared_ptr<T> DataPtr;

	DataHandler() :
		Handler(Handlers::DATA) {
	}
	virtual ~DataHandler() {
	}

	virtual void internal_notify(RSBEventPtr e) {
		boost::shared_ptr<T> d = boost::static_pointer_cast<T>(e->getData());
		notify(d);
	}

	/**
	 * Called if new data is available.
	 *
	 * @param data data of the desired type
	 */
	virtual void notify(DataPtr data) = 0;

};

}

#endif /* HANDLER_H_ */
