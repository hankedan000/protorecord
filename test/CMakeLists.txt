if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING AND CPPUNIT_FOUND)
	add_executable(ProtorecordTest ProtorecordTest.cpp)
	add_test(NAME ProtorecordTest COMMAND ProtorecordTest)

	target_link_libraries(
		ProtorecordTest
			protorecord
			BasicMessage_pb
			${CPPUNIT_LIBRARIES})
endif()
