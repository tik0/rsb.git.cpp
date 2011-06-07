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

#include <rsc/runtime/Printable.h>

#include "../Event.h"
#include "FilterActionTypes.h"
#include "rsb/rsbexports.h"

namespace rsb {
namespace filter {

class FilterObserver;
typedef boost::shared_ptr<FilterObserver> FilterObserverPtr;

/**
 * A filter filters an event stream by removing unwanted events. Therefore
 * it has a function to match an event against its restrictions.
 *
 * As some there may be optimization possible for certain specific filter types,
 * a double-dispatch logic is available using @ref FilterObserver. Instances
 * must implement this #notifyObserver if they are relevant for FilterObserver
 * instances.
 *
 * @note most RSB classes using filters expect that the filters will not change
 *       their configuration after they have been added to e.g. the listener.
 *       So generally do not change a filter after this. Instead first remove
 *       it and then re-add the reconfigured instance.
 * @author swrede
 */
class RSB_EXPORT Filter: public virtual rsc::runtime::Printable {
public:

    Filter();
    virtual ~Filter();

    /**
     * Matches the given event against the constraints specified by this filter.
     *
     * @param e the event to match. Must not be changed!
     * @return @c true if the event matches the restrictions specified by this
     *         filter and hence can be delivered to the client, @c false to
     *         remove the event from the stream.
     */
    virtual bool match(EventPtr e) = 0;

    /**
     * Double-dispatch method to notfify a FilterObserver about changes for this
     * filter with a more specific type that the general Filter interface.
     *
     * The default implementation does not generate a specific notification on
     * FilterObserver. Override this method if there is a specific reception
     * method in FilterObserver.
     *
     * @param fo the observer to notify
     * @param at action that is performed with this filter. Just pass this to
     *           the observer
     */
    virtual void notifyObserver(FilterObserverPtr fo, FilterAction::Types at);

};

typedef boost::shared_ptr<Filter> FilterPtr;

}
}

