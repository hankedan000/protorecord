#include <iostream>
#include "protorecord.h"
#include "DemoMessages.pb.h"

using namespace protorecord;
using namespace protorecord::demo;

int main()
{
	Reader reader("recording");

	while (reader.has_next())
	{
		BasicMessage msg;
		if (reader.take_next(msg))
		{
			std::cout << msg.DebugString() << std::endl;
		}
	}

	return 0;
}