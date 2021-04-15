add_executable(WriterDemo WriterDemo.cpp)
target_link_libraries(WriterDemo
	PUBLIC
		protorecord
		DemoMessages_pb
)

add_executable(ReaderDemo ReaderDemo.cpp)
target_link_libraries(ReaderDemo
	PUBLIC
		protorecord
		DemoMessages_pb
)

add_executable(WriterPerf WriterPerf.cpp)
target_link_libraries(WriterPerf
	PUBLIC
		tqdm
		protorecord
		DemoMessages_pb
)
