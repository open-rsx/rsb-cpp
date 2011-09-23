/* ============================================================
 *
 * This file is a part of RSB project
 *
 * Copyright (C) 2010 by Johannes Wienke <jwienke at techfak dot uni-bielefeld dot de>
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

#include <gmock/gmock.h>

#include <boost/thread.hpp>

#include <rsc/subprocess/Subprocess.h>

#include "testhelpers.h"

#include "rsb/introspection/introspection.h"
#include "rsb/converter/converters.h"

using namespace std;
using namespace testing;
using namespace rsc::subprocess;

int main(int argc, char* argv[]) {

    srand(time(NULL));
	SubprocessPtr spread = startSpread();

	rsb::introspection::registerIntrospectionConverters();
	rsb::converter::registerDefaultConverters();

	InitGoogleMock(&argc, argv);
	return RUN_ALL_TESTS();

}
