#include <iostream>

#include <rsb/Factory.h>

using namespace std;

using namespace boost;

using namespace rsb;

void printEvent(shared_ptr<string> data) {
    cout << "received " << *data << endl;
}

int main() {
    // Setup a listener. This listener uses the default configuration
    // which usually amounts to a sole connector for the spread
    // transport. This connector enables the listener to receive
    // events send by the inprocessAndSpread program.
    //
    // The expected output is:
    // "received foo"
    ListenerPtr listener
	= Factory::getInstance().createListener(Scope("/tutorial/transports"));
    listener->addHandler(HandlerPtr(new DataFunctionHandler<string>(&printEvent)));

    sleep(10);

    return EXIT_SUCCESS;
}
