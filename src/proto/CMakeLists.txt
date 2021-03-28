create_protobuf_target(ProtorecordTypes.proto)

create_protobuf_target(ProtorecordIndex.proto)
target_link_libraries(ProtorecordIndex_pb ProtorecordTypes_pb )
target_link_libraries(ProtorecordIndex_pb_static ProtorecordTypes_pb_static)

install(
	TARGETS
		ProtorecordIndex_pb_static
		ProtorecordTypes_pb_static
	EXPORT protorecord-targets
	ARCHIVE
		DESTINATION "lib/protorecord"
	LIBRARY
		DESTINATION "lib/protorecord"
	PUBLIC_HEADER
		DESTINATION "include/protorecord"
)