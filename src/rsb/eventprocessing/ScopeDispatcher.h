/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2018 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <vector>
#include <list>
#include <map>
#include <stdexcept>

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "../../rsb/Scope.h"

namespace rsb {
namespace eventprocessing {

/**
 * Maps scopes to sets of sinks of type @a T.
 *
 * Sinks are usually objects to which events are dispatched based on
 * their scopes.
 *
 * @author jmoringe
 */
template <typename T>
class ScopeDispatcher {
public:
    /**
     * Indicates whether there are scopes with associated sinks.
     *
     * @return @true if there are no scopes with associated sinks,
     *         @false otherwise
     */
    bool empty() const {
        return this->sinks.empty();
    }

    /**
     * Returns number of scopes with associated sinks.
     *
     * @return The number of scopes with associated sinks.
     */
    size_t size() const {
        return this->sinks.size();
    }

    /**
     * Associates @a sink with @a scope.
     *
     * @param scope The scope with which @a sink should be associated.
     * @param sink The sink which should be added to the list of sinks
     *             associated to @a scope.
     */
    void addSink(const Scope& scope, const T& sink) {
        this->sinks[scope].push_back(sink);
    }

    /**
     * Removes @a sink from the list of sinks associated to @a scope.
     *
     * @param scope The scope from which @a sink should be removed.
     * @param sink The sink which should be removed from the list of sinks
     *             associated to @a scope.
     */
    void removeSink(const Scope& scope, const T& sink) {
        typename SinkMap::iterator it = this->sinks.find(scope);
        if (it == this->sinks.end()) {
            throw std::logic_error("Should not happen");
        }
        SinkList& sinks = it->second;
        sinks.remove(sink);
        if (sinks.empty()) {
            this->sinks.erase(it);
        }
    }

    /**
     * Calls @a function for each sink associated to scope-scopes of @a scope.
     *
     * Relevant sinks are computed by taking the union of the sinks
     * respectively associated to all super-scopes of @a scope.
     *
     * @param scope Determines relevant sinks.
     * @param function The function that should be called for each
     *                 relevant sink.
     */
    void mapSinks(const Scope&                     scope,
                  boost::function<void (const T&)> function) const {
        std::vector<Scope> scopes = scope.superScopes(true);
        for (std::vector<Scope>::const_iterator it = scopes.begin(); it != scopes.end(); ++it) {
            typename SinkMap::const_iterator it_ = this->sinks.find(*it);
            if (it_ != this->sinks.end()) {
                const SinkList& sinks = it_->second;

                for (typename SinkList::const_iterator it__ = sinks.begin();
                     it__ != sinks.end(); ++it__) {
                    function(*it__);
                }
            }
        }
    }
protected:
    typedef std::list<T>              SinkList;
    typedef std::map<Scope, SinkList> SinkMap;

    SinkMap sinks;
};

/**
 * A variant of @ref ScopeDispatcher that references sinks weakly.
 *
 * @author jmoringe
 */
template <typename T>
class WeakScopeDispatcher : public ScopeDispatcher< boost::weak_ptr<T> > {
public:
    void removeSink(const Scope& scope, const T* sink) {
        typename WeakScopeDispatcher::SinkMap::iterator it = this->sinks.find(scope);
        if (it == this->sinks.end()) {
            throw std::logic_error("Should not happen");
        }
        typename WeakScopeDispatcher::SinkList& sinks = it->second;
        for (typename WeakScopeDispatcher::SinkList::iterator it
                 = sinks.begin(); it != sinks.end();) {
            boost::shared_ptr<T> pointer = it->lock();
            // If the weak pointer is dangling, clean it up.
            if (!pointer) {
                it = sinks.erase(it);
            } else if (pointer.get() == sink) {
                it = sinks.erase(it);
            } else {
                ++it;
            }
        }
        if (sinks.empty()) {
            this->sinks.erase(it);
        }
    }

    void mapSinks(const Scope&               scope,
                  boost::function<void (T&)> function) const {
        std::vector<Scope> scopes = scope.superScopes(true);
        for (std::vector<Scope>::const_iterator it = scopes.begin(); it != scopes.end(); ++it) {
            typename WeakScopeDispatcher::SinkMap::const_iterator it_ = this->sinks.find(*it);
            if (it_ != this->sinks.end()) {
                const typename WeakScopeDispatcher::SinkList& sinks = it_->second;

                for (typename WeakScopeDispatcher::SinkList::const_iterator it__
                         = sinks.begin(); it__ != sinks.end(); ++it__) {
                    boost::shared_ptr<T> pointer = it__->lock();
                    if (pointer) {
                        function(*pointer);
                    }
                }
            }
        }
    }
};

}
}
