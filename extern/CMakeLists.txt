# check to see that tqdm was cloned successfully
find_path(TQDM_INCLUDE_DIR
	NAMES tqdm.h
	HINTS "${CMAKE_CURRENT_SOURCE_DIR}/cpptqdm")
if (TQDM_INCLUDE_DIR)
	add_library(tqdm INTERFACE)
	target_include_directories(tqdm INTERFACE ${TQDM_INCLUDE_DIR})
endif()
