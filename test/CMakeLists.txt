if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING AND CPPUNIT_FOUND)
	if (CPPUNIT_FOUND)
		add_executable(ProtorecordTest ProtorecordTest.cpp)
		add_test(NAME ProtorecordTest COMMAND ProtorecordTest)

		target_link_libraries(
			ProtorecordTest
				protorecord
				DemoMessages_pb
				${CPPUNIT_LIBRARIES})
	endif()

	if (Python_FOUND)
		# test python support
		add_test (NAME python-tests
			COMMAND ${Python_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/ProtorecordTest.py"
			WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}/src/py"
		)
	endif()
endif()
