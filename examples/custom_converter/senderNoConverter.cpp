/* ============================================================
 *
 * This file is part of the RSB project
 *
 * Copyright (C) 2011 Jan Moringen <jmoringe@techfak.uni-bielefeld.de>
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

#include <boost/shared_ptr.hpp>

#include <rsb/Factory.h>

#include "SimpleImage.h"

using namespace boost;

using namespace rsb;

using namespace converter_tutorial;

// This program demonstrates the effect of using a data-type for which
// no converter is available (or at least not registered).
int main() {
    Informer<SimpleImage>::Ptr informer =
            Factory::getInstance().createInformer<SimpleImage> (
                    Scope("/tutorial/converter"));

    shared_ptr<SimpleImage> image(new SimpleImage());
    image->width = 10;
    image->height = 10;
    image->data = new unsigned char[100];
    informer->publish(image);

    return EXIT_SUCCESS;
}
