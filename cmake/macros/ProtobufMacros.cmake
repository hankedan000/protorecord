macro (create_protobuf_target PROTO_FILE)
	# generate C++ source from proto source
	protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILE})
	
	# create a library target for the protobuf library
	get_filename_component(PROTO_TARGET ${PROTO_FILE} NAME_WE)
	set(PROTO_TARGET_SHARED "${PROTO_TARGET}_pb")
	set(PROTO_TARGET_STATIC "${PROTO_TARGET}_pb_static")
	add_library(${PROTO_TARGET_SHARED} SHARED ${PROTO_SRCS})
	add_library(${PROTO_TARGET_STATIC} STATIC ${PROTO_SRCS})
	target_link_libraries(${PROTO_TARGET_SHARED} ${Protobuf_LIBRARIES})
	target_link_libraries(${PROTO_TARGET_STATIC} ${Protobuf_LIBRARIES})
	target_include_directories(${PROTO_TARGET_SHARED} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
	target_include_directories(${PROTO_TARGET_STATIC} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
endmacro()
