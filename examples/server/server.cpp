/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <rsb/patterns/RemoteServer.h>
#include <rsb/patterns/Server.h>
#include <rsb/Factory.h>

#include <rsc/logging/LoggerFactory.h>

using namespace std;
using namespace rsb;
using namespace rsb::patterns;

class TestCallback: public Server::Callback<string, string> {
public:

    string methodName;

    TestCallback(const string &methodName) :
        methodName(methodName) {
    }

    string getRequestType() const {
        return "string";
    }

    string getReplyType() const {
        return "string";
    }

    boost::shared_ptr<string> call(const string &/*methodName*/,
            boost::shared_ptr<string> input) {
        return boost::shared_ptr<string>(
                new string("reply to '" + *input + "'"));
    }

};

int main(int /*argc*/, char **/*argv*/) {

    Factory &factory = Factory::getInstance();

    rsc::logging::LoggerFactory::getInstance()->reconfigure(
            rsc::logging::Logger::FATAL);

    const Scope scope("/sxy");
    ServerPtr server = factory.createServer(scope);

    const string methodName1 = "methodOne";
    Server::CallbackPtr m1(new TestCallback(methodName1));

    const string methodName2 = "methodTwo";
    Server::CallbackPtr m2(new TestCallback(methodName2));

    server->registerMethod(methodName1, m1);
    server->registerMethod(methodName2, m2);

    RemoteServerPtr remoteServer = factory.createRemoteServer(scope);

    int iteration = 1;
    while (true) {
        //usleep(500000);

        cout << "++++++++++++ new iteration" << endl;

        EventPtr request1(new Event);
        request1->setType("string");
        stringstream s1;
        s1 << "This is request 1 in iteration " << iteration;
        request1->setData(VoidPtr(new string(s1.str())));
        cout << "+++ Calling method " << methodName1 << endl;
        try {
            EventPtr result = remoteServer->callMethod(methodName1, request1);
            cout << "+++ got result: " << *result << ": "
                    << *(boost::static_pointer_cast<string>(result->getData()))
                    << endl;
        } catch (exception &e) {
            cerr << "+++ Error calling method: " << e.what() << endl;
        }

        EventPtr request2(new Event);
        request2->setType("string");
        stringstream s2;
        s2 << "This is request 2 in iteration " << iteration;
        request2->setData(VoidPtr(new string(s2.str())));
        cout << "+++ Calling method " << methodName2 << endl;
        try {
            EventPtr result = remoteServer->callMethod(methodName2, request2);
            cout << "+++ got result: " << *result << ": "
                    << *(boost::static_pointer_cast<string>(result->getData()))
                    << endl;
        } catch (exception &e) {
            cerr << "+++ Error calling method: " << e.what() << endl;
        }

        ++iteration;

        if (iteration == 10000) {
            break;
        }

    }

}

