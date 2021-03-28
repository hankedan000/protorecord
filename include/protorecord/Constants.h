#pragma once

// size in bytes of the Version message
#define PROTORECORD_VERSION_SIZE 24

// size in bytes of the IndexSummary message
#define PROTORECORD_INDEX_SUMMARY_SIZE 28

// size in bytes of the IndexItem message
#define PROTORECORD_INDEX_ITEM_SIZE_TIMESTAMP 20

// size in bytes of the IndexItem message with no timestamp
#define PROTORECORD_INDEX_ITEM_SIZE_NO_TIMESTAMP 12

namespace protorecord
{
	namespace Flags
	{
		// set if the other bits in the words are valid
		const uint32_t VALID = 0x1;

		// set if the record experienced an error at some point
		// during the recording.
		const uint32_t RECORD_WRITE_ERROR = 0x2;

		// set if the record contains binary message data that was assumed
		// to be serialized externally to library. This flag is set the first
		// time Writer::write_assumed() method is called.
		const uint32_t HAS_ASSUMED_DATA = 0x4;

		// indicating the record contains timestamped items
		const uint32_t HAS_TIMESTAMPS = 0x8;
	}
}
