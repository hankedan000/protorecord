create_protobuf_target(ProtorecordTypes.proto)

create_protobuf_target(ProtorecordIndex.proto)
target_link_libraries(ProtorecordIndex_pb ProtorecordTypes_pb )
target_link_libraries(ProtorecordIndex_pb_static ProtorecordTypes_pb_static)

# generate python source
protobuf_generate_python(PROTO_PY
	ProtorecordTypes.proto
	ProtorecordIndex.proto)

# add custom target to trigger python protobuf generation
add_custom_target(PyProtorecord ALL
	DEPENDS ${PROTO_PY})

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