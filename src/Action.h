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

#ifndef ACTION_H_
#define ACTION_H_

#include "RSBEvent.h"

#include <boost/function.hpp>

namespace rsb {

typedef boost::function<void(RSBEventPtr)> Action;
// TODO: Check if it makes sense to wrap boost::function in a shared_ptr?

}

#endif /* ACTION_H_ */
