/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#pragma once

#include <string>

#include <rsc/runtime/Properties.h>
#include <rsc/threading/TaskExecutor.h>

#include "../../filter/ScopeFilter.h"
#include "../Connector.h"
#include "SpreadConnector.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace spread {

/**
 * @author jmoringe
 */
class RSB_EXPORT InConnector : public rsb::transport::InConnector {
public:
    InConnector(const std::string &host = defaultHost(),
                unsigned int port = defaultPort());

    virtual ~InConnector();

    void activate();
    void deactivate();

    void setQualityOfServiceSpecs(const QualityOfServiceSpec &specs);

    void setObserver(HandlerPtr handler);

    void notify(rsb::filter::ScopeFilter *f,
                const rsb::filter::FilterAction::Types &at);

    static rsb::transport::InConnector* create(const rsc::runtime::Properties &args);
private:
    rsc::logging::LoggerPtr logger;

    bool active;

    SpreadConnectorPtr connector;

    rsc::threading::TaskExecutorPtr exec;
    //   boost::shared_ptr<StatusTask> st;
    boost::shared_ptr<ReceiverTask> rec;

    HandlerPtr observer;
};

}
}