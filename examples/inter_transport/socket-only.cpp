/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011, 2013, 2014 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <iostream>

#include <boost/thread.hpp>

#include <rsc/misc/SignalWaiter.h>

#include <rsb/Factory.h>
#include <rsb/Handler.h>

using namespace std;

using namespace boost;

using namespace rsb;

void printEvent(boost::shared_ptr<string> data) {
    cout << "received " << *data << endl;
}

int main() {

    rsc::misc::initSignalWaiter();

    // Setup a listener. This listener uses the default configuration
    // which usually amounts to a sole connector for the socket
    // transport. This connector enables the listener to receive
    // events send by the inprocess-and-spread program.
    //
    // The expected output is:
    // "received foo"
    ListenerPtr listener
        = getFactory().createListener(Scope("/tutorial/transports"));
    listener->addHandler(
            HandlerPtr(new DataFunctionHandler<string> (&printEvent)));


    return rsc::misc::suggestedExitCode(rsc::misc::waitForSignal());

}
