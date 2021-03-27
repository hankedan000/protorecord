# protorecord
Library for recording protocol buffer messages to disk.

# Building
This project uses a pretty standard cmake build procedure.
```bash
mkdir build
cd build
cmake ..
make
make install
```

# Examples
Some example use cases.

## protorecord::Writer
``` cpp
#include "protorecord.h"
#include "BasicMessage.pb.h"

int main()
{
	protorecord::Writer writer("recording.pr");

	BasicMessage msg;
	msg.set_mystring("helloworld");

	for (unsigned int i=0; i<10; i++)
	{
		msg.set_myint(i);
		writer.write(msg);
	}
}
```

## protorecord::Reader
``` cpp
#include <iostream>
#include "protorecord.h"
#include "BasicMessage.pb.h"

int main()
{
	protorecord::Reader reader("recording.pr");

	while (reader.has_next())
	{
		BasicMessage msg;
		reader.get_next(msg);

		std::cout << msg.DebugString() << std::endl;
	}
}
```
