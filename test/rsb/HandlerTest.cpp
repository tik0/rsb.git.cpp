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

#include "rsb/Handler.h"

#include <iostream>

#include <boost/bind.hpp>

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include <rsc/misc/UUID.h>

#include "rsb/MetaData.h"
#include "rsb/EventId.h"
#include "rsb/Scope.h"

using namespace std;
using namespace rsb;
using namespace testing;

void printEvent(EventPtr e) {
    cout << "Event received: " << e << endl;
}

void printData(boost::shared_ptr<string> e) {
    cout << "Data received: " << *e << endl;
}

TEST(HandlerTest, testDispatch)
{
    HandlerPtr eh(new EventFunctionHandler(boost::bind(&printEvent, _1)));
    HandlerPtr
            dh(new DataFunctionHandler<string> (boost::bind(&printData, _1)));

    EventPtr e(new Event());
    e->setData(boost::shared_ptr<string>(new string("blub")));
    e->setScope(Scope("/blah"));
    e->setType("string");
    e->mutableMetaData().setSenderId(rsc::misc::UUID());

    eh->handle(e);
    dh->handle(e);
}
