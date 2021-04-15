#include "protorecord.h"
#include "DemoMessages.pb.h"

using namespace protorecord;
using namespace protorecord::demo;

int main()
{
	Writer writer("recording");

	BasicMessage msg;
	msg.set_mystring("helloworld");

	for (unsigned int i=0; i<10; i++)
	{
		msg.set_myint(i);
		writer.write(msg);
	}

	return 0;
}