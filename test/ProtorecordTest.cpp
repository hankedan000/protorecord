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
		std::string cmd = "rm -r ";
		cmd += TEST_TMP_PATH;
		system(cmd.c_str());
		if (mkdir(TEST_TMP_PATH.c_str(),0777) < 0)
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
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 10;

		Writer writer(RECORD_PATH);

		BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
		}

		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_timestamps());

		unsigned int expect = 0;
		while (reader.has_next())
		{
			CPPUNIT_ASSERT_MESSAGE(
				"reader ran away!",
				expect < NUM_ITEMS);
			CPPUNIT_ASSERT(reader.get_next(msg));
			CPPUNIT_ASSERT_EQUAL(expect,msg.myint());
			CPPUNIT_ASSERT_EQUAL(std::string("helloworld"),msg.mystring());
			expect++;
		}
	}

	void
	ProtorecordTest::long_write_read()
	{
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 100000;

		Writer writer(RECORD_PATH);

		BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
		}

		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_timestamps());

		unsigned int expect = 0;
		while (reader.has_next())
		{
			CPPUNIT_ASSERT_MESSAGE(
				"reader ran away!",
				expect < NUM_ITEMS);
			CPPUNIT_ASSERT(reader.get_next(msg));
			CPPUNIT_ASSERT_EQUAL(expect,msg.myint());
			CPPUNIT_ASSERT_EQUAL(std::string("helloworld"),msg.mystring());
			expect++;
		}
	}

}// protorecord

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(protorecord::ProtorecordTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
