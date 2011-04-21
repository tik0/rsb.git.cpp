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

#include "Listener.h"

#include <algorithm>
#include <iterator>

#include "filter/ScopeFilter.h"

using namespace std;

namespace rsb {

Listener::Listener(const string &uri) :
	logger(rsc::logging::Logger::getLogger("rsb.Listener")), uri(uri),
			passive(false) {
	this->initialize(transport::Factory::SPREAD, uri);
}

Listener::Listener(transport::Factory::ConnectorTypes in, const string &uri) :
	logger(rsc::logging::Logger::getLogger("rsb.Listener")), uri(uri),
			passive(false) {
	this->initialize(in, uri);
}

Listener::~Listener() {
	if (!passive) {
		deactivate();
	}
}

void Listener::initialize(transport::Factory::ConnectorTypes in,
		const std::string& scope) {
	// TODO evaluate configuration
	this->router = eventprocessing::RouterPtr(
			new eventprocessing::Router(in, transport::Factory::NONE));
	this->subscription.reset(new Subscription());
	this->subscription->appendFilter(
			filter::FilterPtr(new filter::ScopeFilter(scope)));
	this->activate();
}

void Listener::activate() {
	router->activate();
	passive = false;
}

void Listener::deactivate() {
	if (!passive) {
		router->deactivate();
	}
	passive = true;
}

SubscriptionPtr Listener::getSubscription() {
	return this->subscription;
}

void Listener::setSubscription(SubscriptionPtr s) {
	this->router->unsubscribe(this->subscription);
	this->subscription = s;
	this->subscription->appendFilter(
			filter::FilterPtr(new filter::ScopeFilter(this->uri)));
	if (!this->handlers.empty()) {
		this->router->subscribe(this->subscription, this->handlers);
	}
}

set<HandlerPtr> Listener::getHandlers() const {
	set<HandlerPtr> result;
	copy(this->handlers.begin(), this->handlers.end(),
			inserter(result, result.begin()));
	return result;
}

void Listener::appendHandler(HandlerPtr h) {
	if (!this->handlers.empty()) {
		this->router->unsubscribe(this->subscription);
	}
	this->handlers.insert(h);
	this->router->subscribe(this->subscription, this->handlers);
}

void Listener::removeHandler(HandlerPtr h) {
	if (!this->handlers.empty()) {
		this->router->unsubscribe(this->subscription);
	}
	this->handlers.erase(h);
	this->router->subscribe(this->subscription, this->handlers);
}

Listener::Listener() {
}

}