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

#include <boost/shared_ptr.hpp>
#include <boost/function.hpp>

#include "Event.h"
#include "rsb/rsbexports.h"

namespace rsb {

/**
 * A handler is a "callback object" invoked by a @ref rsb::Listener
 * when new data is available that matches the @ref rsb::Subscription
 * associated to the listener.
 *
 * @author swrede
 */
class RSB_EXPORT Handler {
public:
        virtual ~Handler();

        virtual void handle(EventPtr) = 0;
};

typedef boost::shared_ptr<Handler> HandlerPtr;

typedef boost::function<void(EventPtr)> EventFunction;

/**
 * A utility class to simplify event handling in ordinary functions or
 * member functions. This class is primarily intended for use with
 * Boost.Bind.
 */
class RSB_EXPORT EventFunctionHandler : public Handler {
public:
        EventFunctionHandler(const EventFunction& function);

        void handle(EventPtr event);
protected:
        EventFunction function;
};

/**
 * A utility class to simplify data handling by automatically passing
 * the data of the desired type to a function. This class is primarily
 * intended for use with Boost.Bind.
 *
 * @tparam T desired data type to retrieve from the listeners
 *
 * @author swrede
 */
template <typename T>
class DataFunctionHandler : public Handler {
public:
        typedef boost::shared_ptr<T>           DataPtr;
        typedef boost::function<void(DataPtr)> DataFunction;

        DataFunctionHandler(const DataFunction& function)
                : function(function) {
	}

	void handle(EventPtr event) {
                this->function(boost::static_pointer_cast<T>(event->getData()));
	}
protected:
	DataFunction function;
};

}
