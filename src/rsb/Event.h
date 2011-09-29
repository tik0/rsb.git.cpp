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

#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>

#include <boost/cstdint.hpp>
#include <boost/shared_ptr.hpp>

#include <rsc/misc/langutils.h>
#include <rsc/misc/UUID.h>
#include <rsc/runtime/Printable.h>

#include "rsb/rsbexports.h"
#include "EventId.h"
#include "MetaData.h"
#include "Scope.h"

namespace rsb {

typedef boost::shared_ptr<void> VoidPtr;

/**
 * Basic message that is exchanged between informers and listeners. It is a
 * combination of metadata and the actual data to publish / subscribe as
 * payload.
 *
 * @author swrede
 */
class RSB_EXPORT Event: public virtual rsc::runtime::Printable {
public:

    Event();

    /**
     * Constructor.
     *
     * @param scope scope of the event
     * @param payload payload of the event
     * @param type type identifier to serialize / deserialize the event payload
     * @param method a method that is represented by this event. Am empty string
     *               specifies an undecided method.
     */
    Event(ScopePtr scope, boost::shared_ptr<void> payload,
            const std::string &type, const std::string &method = "");

    Event(Scope scope, boost::shared_ptr<void> payload,
            const std::string &type, const std::string &method = "");

    virtual ~Event();

    std::string getClassName() const;
    void printContents(std::ostream &stream) const;

    /**
     * @deprecated use #getEventId instead to access the sequence number.
     */
    DEPRECATED(boost::uint64_t getSequenceNumber() const);

    /**
     * Returns a UUID for the event. This is now generated using the sending
     * participant and a sequence ID. Hence, it is not defined before the event
     * is sent.
     *
     * @return UUID of the event
     * @throw rsc::misc::IllegalStateException if there cannot be an id because
     *                                         the event was not sent so far.
     * @deprecated Use #getEventId instead. This operation is expensive as it
     *             generates a UUID.
     */
    DEPRECATED(rsc::misc::UUID getId() const);
    /**
     * Returns the id of this event. The id is not defined until the event is
     * sent by an informer.
     *
     * @return id of the event
     * @throw rsc::misc::IllegalStateException if there cannot be an id because
     *                                         the event was not sent so far.
     */
    EventId getEventId() const;
    /**
     * Sets all information necessary to generate an id for this event. The
     * senderId will also be set in the meta data of the event.
     *
     * @param senderId id of the sender of this event
     * @param sequenceNumber the unique number per sender this event was sent
     *                       with
     */
    void setEventId(const rsc::misc::UUID &senderId, const boost::uint32_t &sequenceNumber);

    /**
     * @name scope access
     *
     * There are two ways of getting and setting scopes for events. One uses
     * copies of Scopes, which may be expensive and one uses ScopePtr s to
     * avoid the copy operation. Setting the scope with one method makes it also
     * available using the copy getter and vice versa.
     */
    //@{

    ScopePtr getScopePtr() const;
    Scope getScope() const;
    void setScopePtr(ScopePtr scope);
    void setScope(const Scope &scope);

    //@}

    std::string getType() const;
    void setType(const std::string &type);

    VoidPtr getData();
    void setData(VoidPtr d);

    /**
     * Events are often caused by other events, which e.g. means that their
     * contained payload was calculated on the payload of one or more other
     * events.
     *
     * To express these relations each event contains a set of EventIds that
     * express the direct causes of the event. This means, transitive event
     * causes are not modeled.
     *
     * Cause handling is inspired by the ideas proposed in:
     * David Luckham, The Power of Events, Addison-Wessley, 2007
     *
     * @name cause handling
     */
    //@{

    /**
     * Adds the id of one event to the causes of this event. If the set of
     * causing events already contained the given id, this call has no effect.
     *
     * @param id the id of a causing event
     * @return @c true if the causes was added, @c false if it already existed
     */
    bool addCause(const EventId &id);

    /**
     * Removes a causing event from the set of causes for this event. If the
     * id was not contained in this set, the call has no effect.
     *
     * @param id of the causing event
     * @return @c true if an event with this id was removed from the causes,
     *         else @c false
     */
    bool removeCause(const EventId &id);

    /**
     * Tells whether the id of one event is already marked as a cause of this
     * event.
     *
     * @param id id of the event to test causality for
     * @return @c true if @p id is marked as a cause for this event, else
     *         @c false
     */
    bool isCause(const EventId &id) const;

    /**
     * Returns all causing events marked so far.
     *
     * @return set of causing event ids. Modifications to this set do not affect
     *         this event as it is a copy.
     */
    std::set<EventId> getCauses() const;

    //@}

    /**
     * Returns the method associated with this event. An empty string indicates
     * an unspecified method.
     *
     * @return method name or empty string
     */
    std::string getMethod() const;

    /**
     * Sets the method associated with this event.
     *
     * @param method method name or empty string for unspecified method.
     */
    void setMethod(const std::string &method);

    /**
     * @name meta-data access
     */
    //@{

    /**
     * Returns a copy of the current meta-data state of this event.
     *
     * @return copy of event's meta-data
     */
    MetaData getMetaData() const;

    /**
     * Returns an in-place mutable version of the event's meta-data
     *
     * @return meta-data reference to modify
     */
    MetaData &mutableMetaData();

    /**
     * Replaces the event's meta-data with a new instance.
     *
     * @param metaData new meta-data to set
     */
    void setMetaData(const MetaData &metaData);

    //@}

private:

    EventIdPtr id;
    ScopePtr scope;

    VoidPtr content;

    // is this a single type, a hierarchy or a set?
    std::string type;

    std::string method;

    std::set<EventId> causes;

    MetaData metaData;

};

typedef boost::shared_ptr<Event> EventPtr;

}
