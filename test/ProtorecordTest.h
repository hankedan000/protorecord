#pragma once

#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

namespace protorecord
{

	class ProtorecordTest : public CppUnit::TestFixture
	{
		CPPUNIT_TEST_SUITE(ProtorecordTest);
		CPPUNIT_TEST(simple_write_read);
		CPPUNIT_TEST_SUITE_END();

	public:
		ProtorecordTest();
		void setUp();
		void tearDown();

	protected:
		void simple_write_read();

	private:

	};

}