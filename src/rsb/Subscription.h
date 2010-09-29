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

#ifndef SUBSCRIPTION_H_
#define SUBSCRIPTION_H_

#include "RSBEvent.h"
#include "Handler.h"
#include "filter/AbstractFilter.h"

#include <list>
#include <map>
#include <set>
#include <boost/function.hpp>

namespace rsb {

typedef boost::function<void(RSBEventPtr)> Action;
// Q: Does it make sense to wrap boost::function in a shared_ptr?

typedef std::list<rsb::filter::AbstractFilterPtr> FilterChain;
typedef std::list<Action> Actions;

// TODO implement stream operators
// TODO add id field

/**
 * A Subscription, used by a @ref rsb::Subscriber. By appending an @ref rsb::filter::AbstractFilter to the
 * Subscription, one makes sure only events of a certain types are recognized.
 * Appending a Handler is necessary to work with the data received.
 *
 * The following example shows using a Subscription using a ScopeFilter (assume
 * the same uri is used as for the Subscriber) and a QueuePushHandler as a
 * Handler which pushes the received events to a given queue:
 *
 * @code
 * Subscription subscription = new Subscription();
 * AbstractFilterPtr f(new ScopeFilter("rsb://example/informer"));
 * subscription->appendFilter(f);
 * boost::shared_ptr<rsc::SynchronizedQueue<string> > myQueue(new rsc::SynchronizedQueue<string>);
 * boost::shared_ptr<rsb::QueuePushHandler<string> > qph(new QueuePushHandler<string> (myQueue));
 * subscription->appendHandler(qph);
 * @endcode
 *
 *
 * @todo remove header implementations
 * @todo make actions and filter thread-safe, especially for match and action
 *       execution which may be asynchronous to changes in theses subscriptions
 *       I think using a list in a shared_ptr is the wrong way to achieve this.
 */
class Subscription {
public:
	Subscription();
	virtual ~Subscription();

	/**
	 * Appends a Filter to the Subscription. Every event not matching the
	 * constraints provided by the given filter is ignored in processing by the
	 * Handlers.
	 *
	 * @param p a Pointer to the Filter.
	 */
	virtual void appendFilter(rsb::filter::AbstractFilterPtr p);

	virtual bool match(RSBEventPtr e);

	/**
	 * Registers a function bound with boost::bind as a handler.
	 *
	 * @param a boost::bind function to use as a handler
	 */
	virtual void appendAction(Action a);

	/**
	 * Appends a Handler to the Subscription. After events are filtered, all
	 * handlers of a subscription are informed about the event.
	 *
	 * @param h a Pointer to the Handler.
	 */
	virtual void appendHandler(HandlerPtr h);

	// TODO check if it is generally better to return iterators?!
	boost::shared_ptr<FilterChain> getFilters() {
		return filters;
	}

	boost::shared_ptr<std::set<HandlerPtr> > getHandlers() {
		return handlers;
	}

	void removeHandler(HandlerPtr h) {
		handlers->erase(h);
	}

	bool isEnabled() {
		return enabled;
	}

	void disable() {
		enabled = false;
	}

private:
	volatile bool enabled;
	boost::shared_ptr<FilterChain> filters;
	boost::shared_ptr<std::set<HandlerPtr> > handlers;

};

typedef boost::shared_ptr<Subscription> SubscriptionPtr;

}

#endif /* SUBSCRIPTION_H_ */
