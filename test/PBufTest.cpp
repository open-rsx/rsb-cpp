#include <iostream>
#include <fstream>
#include <string>
#include <exception>
#include <boost/shared_ptr.hpp>

#include "protocol/Notification.h"
#include "RSBEvent.h"

using namespace std;
using namespace rsb;
using namespace rsb::protocol;

int main(void) {
	// Verify that the version of the library that we linked against is
	// compatible with the version of the headers we compiled against.
	GOOGLE_PROTOBUF_VERIFY_VERSION;

	RSBEventPtr e(new RSBEvent("xcf://blah",boost::static_pointer_cast<void>(boost::shared_ptr<string>(new string("Hello World!"))),"RSBEventString"));

	// Attention: If an array is set into binary attachment
	//            null values are treated as termination symbols!
	string bin_doc;

	// read XML file with memory element
	try {
		ifstream in("lena.jpg", ios::binary);

		char ch;
		while (in.get(ch)) {
			bin_doc.push_back(ch);
		}
		in.close();
	} catch (std::exception& ex) {
		cerr << ex.what() << endl;
	}

	NotificationPtr n(new Notification());
	n->set_eid("1");
	n->set_s_length(12);
	n->set_standalone(false);
	n->mutable_data()->set_binary(bin_doc);
	// TODO fix this, think about whether this is needed
	n->mutable_data()->set_length(bin_doc.length());

	{
		// Write the notification to disk.
		fstream output("n.bin", ios::out | ios::trunc | ios::binary);
		if (!n->SerializeToOstream(&output)) {
			cerr << "Failed to write notification to stream." << endl;
			return -1;
		}
	}

	{
		n = NotificationPtr(new Notification());

		// Read the notification.
		fstream input("n.bin", ios::in | ios::binary);
		if (!input) {
			cout << "n.bin" << ": File not found.  " << endl;
		} else if (!n->ParseFromIstream(&input)) {
			cerr << "Failed to parse notification." << endl;
			return -1;
		}
		cout << "Parsed event ID: " << n->eid() << endl;
		cout << "Binary length: " << n->data().length() << endl;
		//	cout << "Binary content: " << n->data().binary() << endl;
		const char *b;
		b = n->data().binary().data();

		cout << "Array[0]: " << (char) n->data().binary().data()[0] << endl;

		fstream myfile("lena_copy.jpg", ios::out | ios::trunc | ios::binary);
		// myfile.open();
		myfile << n->data().binary();
		myfile.close();

	}

	// Optional:  Delete all global objects allocated by libprotobuf.
	google::protobuf::ShutdownProtobufLibrary();

	cout << "RoXXor!" << endl; /* prints RoXXor! */
	return 0;
}
