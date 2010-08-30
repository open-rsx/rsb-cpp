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

#include "FilterObservable.h"

namespace rsb {

namespace filter {

FilterObservable::FilterObservable() {
	observers = boost::shared_ptr< std::vector<FilterObserverPtr> >( new std::vector<FilterObserverPtr>() );

}

FilterObservable::~FilterObservable() {
	// TODO Auto-generated destructor stub
}

void FilterObservable::notifyObservers(AbstractFilterPtr af, FilterAction::Types at) {
	for(unsigned int i = 0; i < observers->size(); i++){
		af->notifyObserver((*observers)[i],at);
//		->notify(af.get(),at);
	}
}

void FilterObservable::addObserver(FilterObserverPtr observer) {
	observers->push_back(observer);
}

void FilterObservable::removeObserver(FilterObserverPtr observer) {
	std::vector<FilterObserverPtr>::iterator iter;
	for( iter = observers->begin(); iter != observers->end(); iter++ ) {
		if(*iter == observer){
			observers->erase(iter);
			break;
		}
	}
}

void FilterObservable::clearObservers() {
	observers->clear();
}

}

}
