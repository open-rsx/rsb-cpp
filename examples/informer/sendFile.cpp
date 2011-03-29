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

#include <iostream>
#include <fstream>

#include <stdlib.h>
#include <math.h>
#include <ctime>

#include <boost/timer.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <rsc/logging/Logger.h>

#include <rsb/Publisher.h>
#include <rsb/RSBFactory.h>

using namespace std;
using namespace rsc::logging;
using namespace rsc::misc;
using namespace rsb;
using namespace boost::posix_time;

int main(void) {
  
	RSBFactory factory;
  unsigned int numMsg;
  string *bin_doc = new string();
  string fileLoc;
  
	LoggerPtr l = Logger::getLogger("informer");
  cout << "Please enter valid filename: ";
  getline (cin, fileLoc);
  cout << "Please enter number of messages: ";
  cin >> numMsg;
 
  // Split into filename
  size_t found;
  found=fileLoc.find_last_of("/");
  string file = fileLoc.substr(found+1);

  // Read file
  try {
    ifstream in(fileLoc.c_str(), ios::binary);
    
    char ch;                             
    while (in.get(ch)) {
      bin_doc->push_back(ch);
    }                             
    in.close();
  } catch (std::exception& ex) {
    cerr << ex.what() << endl;
  }
  
  // Create Publisher and RSBEvent
  Publisher<string>::Ptr informer = factory.createPublisher<string>("rsb://example/informer", "string");
  Publisher<string>::DataPtr data(bin_doc);
  
  RSBEventPtr event = RSBEventPtr(new RSBEvent("rsb://example/informer", boost::static_pointer_cast<void>(data), "string"));
  event->addMetaInfo("file", file);
  for (int j = 0; j < numMsg; j++) {
    // Get current time
    event->addMetaInfo("startTime", to_iso_string(microsec_clock::local_time()), true);
    
    // Send event
		informer->publish(event);
#ifndef WIN32
    usleep(6000);
#endif
  }
  cout << "READY" << endl;
	return EXIT_SUCCESS;
}

