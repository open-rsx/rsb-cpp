/*
 * TransportFactoryTest.cpp
 *
 *  Created on: 03.08.2010
 *      Author: swrede
 */

#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>

#include <rsb/transport/AbstractConverter.h>
#include <rsb/transport/converter/ALImageConverter.h>

#include <opencv/highgui.h>

using namespace std;
using namespace rsb;
using namespace rsb::transport;
using namespace AL;

typedef boost::shared_ptr<AbstractConverter<string> > StringConverterPtr;

int main(void) {

	vector<int> resolutions;
	resolutions.push_back(kQQVGA);
	resolutions.push_back(kQVGA);
	resolutions.push_back(kVGA);

	for (vector<int>::iterator resIt = resolutions.begin(); resIt
			!= resolutions.end(); ++resIt) {

		int resolution = *resIt;
		int width, height;
		getSizeFromResolution(resolution, width, height);

		ALImageConverter c;

		IplImage *iplImage = cvCreateImage(cvSize(width, height), 8, 3);
		cvSet(iplImage, CV_RGB(0,0,0));
		cvRectangle(iplImage, cvPoint(0, 0), cvPoint(width / 2, height / 2),
				cvScalar(255, 0, 0, 0), 1, 0, 0);
		cvRectangle(iplImage, cvPoint(width / 4, height / 4), cvPoint(3
				* (width / 4), 3 * (height / 4)), CV_RGB(255, 0, 0), CV_FILLED);
		cvShowImage("before", iplImage);
		cvWaitKey(0);

		AL::ALImage *alimage1 = new AL::ALImage(resolution, AL::kBGRColorSpace,
				true);
		alimage1->setData((unsigned char *) iplImage->imageData);

		//	string m((char *) alimage1->getData(),alimage1->getSize());
		//	ALImage *image = new ALImage(kQQVGA, kRGBColorSpace, false);
		//	image->setData((unsigned char*) m.data());
		//
		//	IplImage *iplImage3 = cvCreateImage(cvSize(160, 120), 8, 3);
		//	iplImage3->imageData = (char*) image->getData();
		//
		//	cvShowImage("after Simple Conversion", iplImage3);
		//	cvWaitKey(0);


		boost::shared_ptr<AL::ALImage> sp(alimage1);
		boost::shared_ptr<void> image_u = boost::static_pointer_cast<void>(sp);
		string s;
		c.serialize("ALImage", image_u, s);
		//cout << "Result of serialize call: " << s << endl;

		boost::shared_ptr<AL::ALImage> alimage2 = boost::static_pointer_cast<
				AL::ALImage>(c.deserialize("ALImage", s));

		IplImage *iplImage2 = cvCreateImage(cvSize(width, height), 8, 3);
		iplImage2->imageData = (char*) alimage2->getData();

		cvShowImage("after", iplImage2);
		cvWaitKey(0);

		//cout << "Result of deserialize call: " << p2->toXML() << endl;

	}

}
