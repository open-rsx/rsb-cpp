#include <boost/shared_ptr.hpp>

#include <rsb/Factory.h>

#include "SimpleImage.h"

using namespace boost;

using namespace rsb;

using namespace converter_tutorial;

// This program demonstrates the effect of using a data-type for which
// no converter is available.
int main() {
    Informer<SimpleImage>::Ptr informer
	= Factory::getInstance().createInformer<SimpleImage>(Scope("/tutorial/converter"));

    shared_ptr<SimpleImage> image(new SimpleImage());
    image->width = 10;
    image->height = 10;
    image->data = new unsigned char[100];
    informer->publish(image);

    return EXIT_SUCCESS;
}
