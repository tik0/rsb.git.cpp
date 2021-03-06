/* ============================================================
 *
 * This file is a part of the RSB project
 *
 * Copyright (C) 2010 by Sebastian Wrede <swrede at techfak dot uni-bielefeld dot de>
 *
 * This file may be licensed under the terms of the
 * GNU Lesser General Public License Version 3 (the ``LGPL''),
 * or (at your option) any later version.
 *
 * Software distributed under the License is distributed
 * on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
 * express or implied. See the LGPL for the specific language
 * governing rights and limitations.
 *
 * You should have received a copy of the LGPL along with this
 * program. If not, go to http://www.gnu.org/licenses/lgpl.html
 * or write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 *
 * The development of this software was supported by:
 *   CoR-Lab, Research Institute for Cognition and Robotics
 *     Bielefeld University
 *
 * ============================================================ */

#pragma once

#include <boost/shared_ptr.hpp>

#include <rsc/logging/Logger.h>
#include <rsc/runtime/Printable.h>

#include "FilterActionTypes.h"

namespace rsb {
namespace filter {

class Filter;
class ScopeFilter;

/**
 * @author swrede
 * @todo Check if Double Dispatch pattern is best suited here
 */
class RSB_EXPORT FilterObserver: public virtual rsc::runtime::Printable {
public:
    FilterObserver();
    virtual ~FilterObserver();

    virtual void notify(Filter* filter, const FilterAction::Types& at);

    virtual void notify(ScopeFilter* filter, const FilterAction::Types& at);

private:

    rsc::logging::LoggerPtr logger;

};

typedef boost::shared_ptr<FilterObserver> FilterObserverPtr;

}
}
