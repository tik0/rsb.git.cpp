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

#pragma once

#include <string>
#include <vector>

#include <boost/shared_ptr.hpp>
#include <boost/thread/recursive_mutex.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/TypeStringTools.h>
#include <rsc/patterns/Singleton.h>

#include "rsb/rsbexports.h"
#include "Reader.h"
#include "Listener.h"
#include "Informer.h"
#include "patterns/Server.h"
#include "patterns/RemoteServer.h"
#include "transport/Connector.h"
#include "ParticipantConfig.h"
#include "Service.h"

namespace rsb {

/**
 * Factory for RSB user-level domain objects for communication patterns.
 *
 * @author jwienke
 */
class RSB_EXPORT Factory: public rsc::patterns::Singleton<Factory> {
public:

    virtual ~Factory();

    /**
     * Creates a new informer in the specified scope.
     *
     * @tparam DataType the C++ data type this informer publishes
     * @param scope the scope of the informer
     * @param config the configuration for the informer to use, defaults to the
     *               general config held in this factory.
     * @param dataType the string representation of the data type used to select
     *                 converters
     * @return new informer instance
     */
    template<class DataType>
    typename Informer<DataType>::Ptr createInformer(const Scope &scope,
            const ParticipantConfig &config =
                    Factory::getInstance().getDefaultParticipantConfig(),
            const std::string &dataType = rsc::runtime::typeName(
                    typeid(DataType))) {
        return typename Informer<DataType>::Ptr(new Informer<DataType> (
            createOutConnectors(config), scope, config, dataType));
    }

    /**
     * Creates a new listener for the specified scope.
     *
     * @param scope the scope of the new listener
     * @param config the configuration for the LISTENER to use, defaults to the
     *               general config held in this factory.f
     * @return new listener instance
     */
    ListenerPtr createListener(const Scope &scope,
            const ParticipantConfig &config =
                    Factory::getInstance().getDefaultParticipantConfig());

    /**
     * Creates a new @ref Reader object for the specified scope.
     *
     * @ref Reader objects receive event via a pull-style interface
     * by calls to @ref Reader::read.
     *
     * @param scope the scope of the new receiver
     * @throw RSBError when the requested connection cannot be
     * established.
     * @return A shared_ptr to the new @ref Reader object.
     **/
    ReaderPtr createReader(const Scope &scope,
                           const ParticipantConfig &config =
                           Factory::getInstance().getDefaultParticipantConfig());

    /**
     * Creates a Service instance operating on the given scope.
     *
     * @param scope parent-scope of the new service
     * @return new service instance
     */
    ServicePtr createService(const Scope &scope);

    patterns::ServerPtr createServer(const Scope &scope);


    /**
     * Creates a @ref RemoteServer object for the server at scope @a
     * scope.
     *
     * @param scope The scope at which the remote server object
     * exposes itself.
     * @param maxReplyWaitTime Maximum number of seconds to wait for a
     * reply from the server when calling a method.
     * @return A shared_ptr to the new @ref RemoteServer object.
     */
    patterns::RemoteServerPtr createRemoteServer(const Scope &scope,
                                                 unsigned int maxReplyWaitTime = 25);

    /**
     * Returns the default configuration for new participants.
     *
     * @return copy of the default configuration
     */
    ParticipantConfig getDefaultParticipantConfig() const;

    /**
     * Sets the default config for participants that will be used for every new
     * participant that is created after this call.
     *
     * @param config new config
     */
    void setDefaultParticipantConfig(const ParticipantConfig &config);

    friend class rsc::patterns::Singleton<Factory>;

private:

    /**
     * This function is only required to have the same factory instance on
     * windows because there the Singleton template is instantiated once per
     * compilation unit. For the template-based createInformer method this will
     * then be in the caller's compilation unit. :/
     */
    transport::OutFactory &getOutFactoryInstance();

    /**
     * Singleton constructor.
     */
    Factory();

    rsc::logging::LoggerPtr logger;

    /**
     * Always acquire configMutex before reading or writing the config.
     */
    ParticipantConfig defaultConfig;
    mutable boost::recursive_mutex configMutex;

    std::vector<transport::OutConnectorPtr>
        createOutConnectors(const ParticipantConfig &config);

    std::vector<transport::InPullConnectorPtr>
        createInPullConnectors(const ParticipantConfig &config);

    std::vector<transport::InPushConnectorPtr>
        createInPushConnectors(const ParticipantConfig &config);

};

}
