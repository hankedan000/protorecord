add_subdirectory(proto)

add_executable(WriterDemo WriterDemo.cpp)
target_link_libraries(WriterDemo
	PUBLIC
		protorecord
		BasicMessage_pb
)

add_executable(ReaderDemo ReaderDemo.cpp)
target_link_libraries(ReaderDemo
	PUBLIC
		protorecord
		BasicMessage_pb
)