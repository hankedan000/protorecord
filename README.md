# protorecord
Small library for recording protocol buffer messages to disk.

# Building
This project uses a pretty standard cmake build procedure.
```bash
mkdir build
cd build
cmake ..
make
make install
```

# Writer
A class that stores protobuf messages to a record.

``` cpp
#include "protorecord.h"
#include "BasicMessage.pb.h"

int main()
{
   protorecord::Writer writer("recording");
   
   for (unsigned int i=0; i<10; i++)
   {
      BasicMessage msg;
      msg.set_mystring("helloworld");
      msg.set_myint(i);
      
      writer.write(msg);
   }
   
   return 0;
}
```

# Reader
A class that reads protobuf messages from a record.

``` cpp
#include <iostream>
#include "protorecord.h"
#include "BasicMessage.pb.h"

int main()
{
   protorecord::Reader reader("recording");
   
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
```
