create_protobuf_target(Protorecord.proto)
create_protobuf_target(DemoMessages.proto)

install(
	TARGETS
		Protorecord_pb_static
	EXPORT protorecord-targets
	ARCHIVE
		DESTINATION "lib/protorecord"
	LIBRARY
		DESTINATION "lib/protorecord"
	PUBLIC_HEADER
		DESTINATION "include/protorecord"
)