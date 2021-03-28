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
			CPPUNIT_ASSERT(reader.take_next(msg));
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
			CPPUNIT_ASSERT(reader.take_next(msg));
			CPPUNIT_ASSERT_EQUAL(expect,msg.myint());
			CPPUNIT_ASSERT_EQUAL(std::string("helloworld"),msg.mystring());
			expect++;
		}
	}

	void
	ProtorecordTest::version()
	{
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 0;

		Writer writer(RECORD_PATH);
		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());

		Version actual = reader.get_version();
		CPPUNIT_ASSERT_EQUAL((int)PROTORECORD_VERSION_MAJOR,(int)actual.major());
		CPPUNIT_ASSERT_EQUAL((int)PROTORECORD_VERSION_MINOR,(int)actual.minor());
		CPPUNIT_ASSERT_EQUAL((int)PROTORECORD_VERSION_PATCH,(int)actual.patch());

		// make sure version is 0.0.0 when record doesn't exist
		Reader reader_no_version("this_record_is_expected_to_not_exist");
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader_no_version.size());

		actual = reader_no_version.get_version();
		CPPUNIT_ASSERT_EQUAL((int)0,(int)actual.major());
		CPPUNIT_ASSERT_EQUAL((int)0,(int)actual.minor());
		CPPUNIT_ASSERT_EQUAL((int)0,(int)actual.patch());
	}

	void
	ProtorecordTest::timestamping()
	{
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 100;
		const unsigned int SLEEP_INTERVAL = 10000;

		Writer writer(RECORD_PATH,true);// enable timestamping

		BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
			usleep(SLEEP_INTERVAL);// wait ~100ms between writes
		}

		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(true,reader.has_timestamps());

		uint64_t start_time_us;
		CPPUNIT_ASSERT(reader.get_start_time(start_time_us));
		CPPUNIT_ASSERT(start_time_us > 0);

		uint32_t expect_item_num = 0;
		uint64_t prev_timestamp = 0;
		bool first_item = true;
		while (reader.has_next())
		{
			CPPUNIT_ASSERT_MESSAGE(
				"reader ran away!",
				expect_item_num < NUM_ITEMS);

			// check item timestamp
			uint64_t actual_timestamp;
			CPPUNIT_ASSERT(reader.get_next_timestamp(actual_timestamp));
			if (first_item)
			{
				// make sure expected value is close to zero
				CPPUNIT_ASSERT_DOUBLES_EQUAL(0ULL,actual_timestamp,1000.0);
			}
			else
			{
				double diff = actual_timestamp - prev_timestamp;
				// make sure expected value is within 1ms
				CPPUNIT_ASSERT_DOUBLES_EQUAL(SLEEP_INTERVAL,diff,1000.0);
			}
			prev_timestamp = actual_timestamp;

			CPPUNIT_ASSERT(reader.take_next(msg));
			CPPUNIT_ASSERT_EQUAL(expect_item_num,msg.myint());
			CPPUNIT_ASSERT_EQUAL(std::string("helloworld"),msg.mystring());

			expect_item_num++;
			first_item = false;
		}
	}

}// protorecord

int main()
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(protorecord::ProtorecordTest::suite());
	return runner.run() ? 0 : EXIT_FAILURE;
}
