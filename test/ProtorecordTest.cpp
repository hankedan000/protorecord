#include "ProtorecordTest.h"

#include <sys/stat.h>
#include <unistd.h>
#include "DemoMessages.pb.h"
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

		protorecord::demo::BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());

		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_timestamps());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_assumed_data());

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

		protorecord::demo::BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());

		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_timestamps());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_assumed_data());

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
	ProtorecordTest::assumed_write_read()
	{
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 10;

		Writer writer(RECORD_PATH);

		uint8_t buffer[1024];
		protorecord::demo::BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(msg.SerializeToArray((void*)buffer,sizeof(buffer)));
			CPPUNIT_ASSERT(writer.write_assumed((void*)buffer,msg.ByteSizeLong()));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());

		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_timestamps());
		CPPUNIT_ASSERT_EQUAL(true,reader.has_assumed_data());

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
	ProtorecordTest::overwrite()
	{
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 10;

		Writer writer1(RECORD_PATH);

		protorecord::demo::BasicMessage msg;
		msg.set_mystring("write1");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer1.write(msg));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer1.size());

		writer1.close();

		// ----------------------

		Writer writer2(RECORD_PATH);
		msg.set_mystring("write2");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer2.write(msg));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer2.size());

		writer2.close();

		// ----------------------

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_timestamps());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_assumed_data());

		unsigned int expect = 0;
		while (reader.has_next())
		{
			CPPUNIT_ASSERT_MESSAGE(
				"reader ran away!",
				expect < NUM_ITEMS);
			CPPUNIT_ASSERT(reader.take_next(msg));
			CPPUNIT_ASSERT_EQUAL(expect,msg.myint());
			CPPUNIT_ASSERT_EQUAL(std::string("write2"),msg.mystring());
			expect++;
		}
	}

	void
	ProtorecordTest::late_opened_writer()
	{
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 10;

		Writer writer;

		// open the Writer after construction (ie "late")
		CPPUNIT_ASSERT(writer.open(RECORD_PATH));

		protorecord::demo::BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());

		writer.close();

		Reader reader(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader.size());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_timestamps());
		CPPUNIT_ASSERT_EQUAL(false,reader.has_assumed_data());

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
	ProtorecordTest::reused_writer()
	{
		const std::string RECORD1_PATH(TEST_TMP_PATH + "/" + __func__ + "1");
		const std::string RECORD2_PATH(TEST_TMP_PATH + "/" + __func__ + "2");
		const size_t NUM_ITEMS = 10;

		Writer writer;

		// open the writer for the first record
		CPPUNIT_ASSERT(writer.open(RECORD1_PATH));

		protorecord::demo::BasicMessage msg;
		msg.set_mystring("this is record1");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());

		writer.close();

		// ----------------------

		Reader reader1(RECORD1_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader1.size());
		CPPUNIT_ASSERT_EQUAL(false,reader1.has_timestamps());
		CPPUNIT_ASSERT_EQUAL(false,reader1.has_assumed_data());

		unsigned int expect = 0;
		while (reader1.has_next())
		{
			CPPUNIT_ASSERT_MESSAGE(
				"reader ran away!",
				expect < NUM_ITEMS);
			CPPUNIT_ASSERT(reader1.take_next(msg));
			CPPUNIT_ASSERT_EQUAL(expect,msg.myint());
			CPPUNIT_ASSERT_EQUAL(std::string("this is record1"),msg.mystring());
			expect++;
		}

		// ----------------------------------------

		// open the writer for the second record
		CPPUNIT_ASSERT(writer.open(RECORD2_PATH));

		msg.set_mystring("this is record2");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
		}

		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());

		writer.close();

		// ----------------------

		Reader reader2(RECORD2_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,reader2.size());
		CPPUNIT_ASSERT_EQUAL(false,reader2.has_timestamps());
		CPPUNIT_ASSERT_EQUAL(false,reader2.has_assumed_data());

		expect = 0;
		while (reader2.has_next())
		{
			CPPUNIT_ASSERT_MESSAGE(
				"reader ran away!",
				expect < NUM_ITEMS);
			CPPUNIT_ASSERT(reader2.take_next(msg));
			CPPUNIT_ASSERT_EQUAL(expect,msg.myint());
			CPPUNIT_ASSERT_EQUAL(std::string("this is record2"),msg.mystring());
			expect++;
		}
	}

	void
	ProtorecordTest::version()
	{
		const std::string RECORD_PATH(TEST_TMP_PATH + "/" + __func__);
		const size_t NUM_ITEMS = 0;

		Writer writer(RECORD_PATH);
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());
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

		protorecord::demo::BasicMessage msg;
		msg.set_mystring("helloworld");

		for (unsigned int i=0; i<NUM_ITEMS; i++)
		{
			msg.set_myint(i);
			CPPUNIT_ASSERT(writer.write(msg));
			usleep(SLEEP_INTERVAL);// wait ~100ms between writes
		}
		
		CPPUNIT_ASSERT_EQUAL(NUM_ITEMS,writer.size());

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
