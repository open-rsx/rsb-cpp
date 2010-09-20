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

#include "EventProcessor.h"
#include "MatchAndExecute.h"

using namespace std;

namespace rsb {

namespace internal {

EventProcessor::EventProcessor() : pool(5) {

}

EventProcessor::EventProcessor(unsigned int num_threads) : pool(num_threads) {

}

EventProcessor::~EventProcessor() {
	// TODO Auto-generated destructor stub
}


void EventProcessor::process(RSBEventPtr e) {
	// TODO subscriptions needs to be made thread-safe
    list<SubscriptionPtr>::iterator i = subscriptions.begin();
    while(i != subscriptions.end()) {
        // execute match-and-trigger if enabled, otherwise remove
        if((*i)->isEnabled()) {
        		pool.schedule(boost::bind(&MatchAndExecute::process, matcher, *i, e));
        		++i;
        } else {
        	// actually removal of marked subscription
        	i = subscriptions.erase(i);
        }
    }
}

void EventProcessor::subscribe(SubscriptionPtr s) {
	subscriptions.push_back(s);
}
void EventProcessor::unsubscribe(SubscriptionPtr s) {
    list<SubscriptionPtr>::iterator i = find(subscriptions.begin(), subscriptions.end(), s);
    if(i != subscriptions.end())
        (*i)->disable();
    // next time they are going to be removed, see process
}

}

}
