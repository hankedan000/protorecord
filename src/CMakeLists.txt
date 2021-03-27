add_subdirectory(proto)

add_library(protorecord SHARED
	Writer.cpp
)
target_link_libraries(protorecord
	PUBLIC
		${Protobuf_LIBRARIES}
	PRIVATE
		ProtorecordIndex_pb_static
)

# build a list of public header file to install
list(APPEND protorecord_PUBLIC_HEADERS
	"${PROTORECORD_PUBLIC_INCLUDE_DIR}/Writer.h"
	"${PROTORECORD_PUBLIC_INCLUDE_DIR}/Utils.h"
)
set_target_properties(protorecord PROPERTIES
	PUBLIC_HEADER "${protorecord_PUBLIC_HEADERS}"
)
install(
	TARGETS protorecord
	LIBRARY
		DESTINATION lib
	PUBLIC_HEADER
		DESTINATION "include/protorecord"
)