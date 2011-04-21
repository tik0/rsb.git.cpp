/* ============================================================
 *
 * This file is a part of the RSB project.
 *
 * Copyright (C) 2011 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include "Factory.h"

#include "introspection/introspection.h"
#include "converter/converters.h"

using namespace std;

namespace rsb {

Factory::Factory() {
	introspection::registerIntrospectionConverters();
	converter::registerDefaultConverters();
}

Factory::~Factory() {
}

ListenerPtr Factory::createListener(const string &uri) {
	return ListenerPtr(new Listener(uri));
}

patterns::ServerPtr Factory::createServer(const string &uri) {
	return patterns::ServerPtr(new patterns::Server(uri));
}

patterns::RemoteServerPtr Factory::createRemoteServer(const string &uri) {
	return patterns::RemoteServerPtr(new patterns::RemoteServer(uri));
}

}