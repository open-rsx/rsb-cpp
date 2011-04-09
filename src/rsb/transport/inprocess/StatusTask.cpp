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

#include "StatusTask.h"

using namespace std;

namespace rsb {
namespace inprocess {

StatusTask::StatusTask(InProcessPort* p, const unsigned int &ms) :
	rsc::threading::PeriodicTask(ms), port(p) {
}

StatusTask::~StatusTask() {
	cout << "dtr ~StatusTask() " << endl;
}

void StatusTask::execute() {
	cout << "InProcessPort alive." << endl;
}

}
}
