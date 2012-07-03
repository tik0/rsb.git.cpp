/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2012 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include "InPushConnector.h"

#include "Bus.h"
#include "../../MetaData.h"

using namespace std;

using namespace rsc::logging;
using namespace rsc::runtime;

namespace rsb {
namespace inprocess {

InPushConnector::InPushConnector() :
    logger(Logger::getLogger("rsb.inprocess.InPushConnector")), active(false) {
}

transport::InPushConnector* InPushConnector::create(const Properties& args) {
    LoggerPtr logger = Logger::getLogger("rsb.inprocess.InPushConnector");
    RSCDEBUG(logger, "Creating InPushConnector with properties " << args);

    // Seems to have confused some users.
    // See https://code.cor-lab.de/issues/649
    // if (args.has("converters")) {
    //   RSCWARN(logger, "`converters' property found when constructing inprocess::InPushConnector. This connector does not support (or require) converters.");
    // }
    return new InPushConnector();
}

InPushConnector::~InPushConnector() {
    if (this->active) {
        deactivate();
    }
}

string InPushConnector::getClassName() const {
    return "InPushConnector";
}

void InPushConnector::printContents(ostream& stream) const {
    stream << "scope = " << scope;
}

Scope InPushConnector::getScope() const {
    return this->scope;
}

void InPushConnector::setScope(const Scope& scope) {
    if (this->active) {
        throw std::runtime_error("Cannot set scope while active");
    }

    this->scope = scope;
}

void InPushConnector::activate() {
    RSCDEBUG(logger, "Activating");
    Bus::getInstance().addSink(boost::dynamic_pointer_cast<InPushConnector>(
            shared_from_this()));
    this->active = true;
}

void InPushConnector::deactivate() {
    RSCDEBUG(logger, "Deactivating");
    Bus::getInstance().removeSink(this);
}

void InPushConnector::setQualityOfServiceSpecs(const QualityOfServiceSpec& /*specs*/) {
}

void InPushConnector::handle(EventPtr event) {

    /** TODO(jmoringe, 2011-11-07): This ensures not overwriting
     * earlier receive timestamp added by different
     * connector. However, thread-safety issue remains.  */
    if (event->getMetaData().getReceiveTime() == 0) {
        event->mutableMetaData().setReceiveTime();
    }
    for (HandlerList::iterator it = this->handlers.begin(); it
            != this->handlers.end(); ++it) {
        (*it)->handle(event);
    }
}

}
}