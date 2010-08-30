/*
 * VideoSubscriber.cpp
 *
 *  Created on: Aug 30, 2010
 *      Author: jseele
 */

#include <stdlib.h>

#include <iostream>

#include <rsb/Subscriber.h>
#include <rsb/Subscription.h>
#include <rsb/Handler.h>
#include <rsb/filter/ScopeFilter.h>
#include <rsb/util/Timer.h>

#include <alvisiondefinitions.h>
#include <alimage.h>
#include <opencv/highgui.h>


using namespace std;
using namespace rsb;
using namespace rsb::filter;
using namespace rsb::util;
using namespace AL;

class MyDataHandler : public DataHandler<ALImage> {
public:
	MyDataHandler() {};

	void notify(boost::shared_ptr<ALImage> e) {
		IplImage *image = cvCreateImage(cvSize(160, 120), 8, 3);
		image->imageData = (char*) e->getData();

		cvShowImage("naocam", image);
		cvWaitKey(1);
	}
};


int main(void) {


    SubscriberPtr s(new Subscriber("Vision Subscriber"));
    SubscriptionPtr sub(new Subscription());
	AbstractFilterPtr f(new ScopeFilter("rsb://nao/vision/top"));
	sub->appendFilter(f);

	boost::shared_ptr<MyDataHandler> dh(new MyDataHandler());
	boost::shared_ptr<Handler> h = boost::static_pointer_cast<Handler>(dh);

	// register event handler
	sub->appendHandler(h);

	s->addSubscription(sub);


	cvWaitKey(0);

    return (EXIT_SUCCESS);
}
