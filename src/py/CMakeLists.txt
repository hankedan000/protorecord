if (Python_FOUND)
	# test python support
	add_test (NAME python-tests
		COMMAND ${Python_EXECUTABLE} "${CMAKE_CURRENT_SOURCE_DIR}/ProtorecordTest.py")
endif()