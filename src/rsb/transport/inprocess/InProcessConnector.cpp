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

#include <iostream>

#include <string.h>

#include <boost/bind.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/threading/ThreadedTaskExecutor.h>

#include "InProcessConnector.h"
#include "../../util/Configuration.h"
#include "../../CommException.h"
#include "../../converter/Converter.h"

using namespace std;
using namespace rsc::logging;
using namespace rsb;
using namespace rsb::util;
using namespace rsb::transport;
using namespace rsc::threading;

namespace {
LoggerPtr logger(Logger::getLogger("rsb.inprocess"));
}

namespace rsb {
namespace inprocess {

InProcessConnector::InProcessConnector() :
	shutdown(false), exec(new ThreadedTaskExecutor),
			st(new StatusTask(this, 500)) {
	// TODO check if it makes sense and is possible to provide a weak_ptr to
	// the ctr of StatusTask
}

InProcessConnector::~InProcessConnector() {
	deactivate();
	cout << "InProcessConnector::~InProcessConnector()" << endl;
}

void InProcessConnector::activate() {
	// (re-)start threads
	exec->schedule(st);
}

void InProcessConnector::deactivate() {
	shutdown = true;
	cout << "stopping st task" << endl;
	st->cancel();
	cout << "InProcessConnector::deactivate finished" << endl;
}

void InProcessConnector::push(EventPtr e) {
	// get matching converter -- can be skipped here
	//	string s;
	//	boost::shared_ptr<void> obj = boost::static_pointer_cast<void>(e->getData());
	//	boost::shared_ptr<Converter<string> > c = boost::static_pointer_cast<Converter<string> >((*converters)["person"]);
	//	c->serialize("person",obj,s);
	//	cout << "Result of serialize1 call: " << s << endl;

	// if (only_id) subscription just call listener directly?!?
	// don't dispatch via matching subsystem?!? otherwise mark at least event for IdFilter as matched
	// localize dispatching in matching subsystem, don't make these ports here to complicated...
	// decide this based on considerations of pattern implementation / configuration examples

	this->observer->handle(e);
}

/**
 * @todo implement this!
 */
void InProcessConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec &/*specs*/) {

}

}
}