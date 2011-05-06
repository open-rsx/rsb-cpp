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

#include "Handler.h"

using namespace std;

namespace rsb {

Handler::Handler(const string &method) {
    methods.insert(method);
}

Handler::Handler(const set<string> &methods) :
    methods(methods) {
}

Handler::~Handler() {
}

set<string> Handler::getMethods() const {
    return methods;
}

EventFunctionHandler::EventFunctionHandler(const EventFunction& function) :
    function(function) {
}

string EventFunctionHandler::getClassName() const {
    return "EventFunctionHandler";
}

void EventFunctionHandler::printContents(ostream &stream) const {
    stream << "function = " << function;
}

void EventFunctionHandler::handle(EventPtr event) {
    this->function(event);
}

}
