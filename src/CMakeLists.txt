add_subdirectory(proto)

add_library(protorecord SHARED
	Writer.cpp
	Reader.cpp
)
target_link_libraries(protorecord
	PUBLIC
		${Protobuf_LIBRARIES}
		Protorecord_pb_static
)

# build a list of public header file to install
list(APPEND protorecord_PUBLIC_HEADERS
	"${PROTORECORD_INCLUDE_DIR}/protorecord/Writer.h"
	"${PROTORECORD_INCLUDE_DIR}/protorecord/Reader.h"
	"${PROTORECORD_INCLUDE_DIR}/protorecord/Constants.h"
	"${PROTORECORD_INCLUDE_DIR}/protorecord/Utils.h"
	"${CMAKE_BINARY_DIR}/include/protorecord/version.h"
)
set_target_properties(protorecord PROPERTIES
	PUBLIC_HEADER "${protorecord_PUBLIC_HEADERS}"
)
install(
	TARGETS protorecord
	EXPORT protorecord-targets
	ARCHIVE
		DESTINATION lib
	LIBRARY
		DESTINATION lib
	PUBLIC_HEADER
		DESTINATION "include/protorecord"
)
