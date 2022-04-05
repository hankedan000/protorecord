#include <iostream>
#include "tqdm.h"
#include "protorecord.h"
#include "DemoMessages.pb.h"

using namespace protorecord;
using namespace protorecord::demo;

int main()
{
	const unsigned int N = 1000000;
	tqdm bar;
	Writer writer("small_recording");

	std::cout << "BasicMessage test" << std::endl;

	BasicMessage bmsg;
	bmsg.set_mystring("helloworld");

	bar.reset();
	for (unsigned int i=0; i<N; i++)
	{
		bar.progress(i, N);
		bmsg.set_myint(rand());
		writer.write(bmsg);
	}
	bar.finish();
	writer.close();

	// --------------------------------------------

	std::cout << "LargeMessage test" << std::endl;

	writer.open("large_recording");

	LargeMessage lmsg;
	lmsg.add_mystrings("helloworld1");
	lmsg.add_mystrings("helloworld2");
	lmsg.add_mystrings("helloworld3");
	lmsg.add_mystrings("helloworld4");
	lmsg.add_myints(rand());
	lmsg.add_myints(rand());
	lmsg.add_myints(rand());
	lmsg.add_myints(rand());
	lmsg.add_mybools(rand()%2);
	lmsg.add_mybools(rand()%2);
	lmsg.add_mybools(rand()%2);
	lmsg.add_mybools(rand()%2);

	bar.reset();
	for (unsigned int i=0; i<N; i++)
	{
		bar.progress(i, N);
		writer.write(lmsg);
	}
	bar.finish();
	writer.close();

	return 0;
}