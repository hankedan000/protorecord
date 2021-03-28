#include "ProtorecordTest.h"

#include <sys/stat.h>
#include <unistd.h>
#include "BasicMessage.pb.h"
#include "protorecord.h"

namespace protorecord
{

	ProtorecordTest::ProtorecordTest()
	{
	}

	void
	ProtorecordTest::setUp()
	{
		// make a clean temporary directory for saving recordings
		int rc = rmdir("test_records");
		if (rc < 0 && errno == ENOENT)
		{
			// case is okay
		}
		else if (rc < 0)
		{
			std::cerr << "failed to clean up previous test_records directory!" << std::endl;
			std::cerr << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
		if (mkdir("test_records",0777) < 0)
		{
			std::cerr << "failed to create test_records directory!" << std::endl;
			std::cerr << strerror(errno) << std::endl;
			exit(EXIT_FAILURE);
		}
	}

	void
	ProtorecordTest::tearDown()
	{
	}

	void
	ProtorecordTest::simple_write_read()
	{
		Writer writer("recording");

		BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<10; i++)
		{
			msg.set_myint(i);
			writer.write(msg);
		}

	}

}// protorecord

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(protorecord::ProtorecordTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
