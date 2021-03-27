add_subdirectory(proto)

add_executable(WriterDemo WriterDemo.cpp)
target_link_libraries(WriterDemo
	PUBLIC
		protorecord
		BasicMessage_pb
)