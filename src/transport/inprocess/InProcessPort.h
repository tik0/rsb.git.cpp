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

#ifndef INPROCESSPORT_H_
#define INPROCESSPORT_H_

#include "../Port.h"
#include "../../util/TaskExecutor.h"
#include "StatusTask.h"
#include "../QueueAndDispatchTask.h"

#include <boost/function.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/thread.hpp>

namespace rsb {

namespace inprocess {

class InProcessPort : public rsb::transport::Port {
public:
	InProcessPort();
	virtual ~InProcessPort();

	void activate();
	void deactivate();
	void push(rsb::RSBEventPtr e);

	// Observer may implement complex event matching
	// or be just directly the user-level event handlers
	void setObserver(rsb::Action a);
	void removeObserver(rsb::Action a);

private:
	volatile bool shutdown;

	rsb::util::TaskExecutorVoidPtr exec;
    boost::shared_ptr<StatusTask> st;
    boost::shared_ptr<rsb::transport::QueueAndDispatchTask<RSBEventPtr> > qad;

	rsb::util::TaskPtr qadTask;
	rsb::util::TaskPtr staTask;
};

}

}

#endif /* INPROCESSPORT_H_ */
