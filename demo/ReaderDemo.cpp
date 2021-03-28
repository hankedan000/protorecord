#include <iostream>
#include "protorecord.h"
#include "BasicMessage.pb.h"

using namespace protorecord;

int main()
{
	Reader reader("recording");

	while (reader.has_next())
	{
		BasicMessage msg;
		reader.get_next(msg);

		std::cout << msg.DebugString() << std::endl;
	}

	return 0;
}