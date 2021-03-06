#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "Protorecord.pb.h"
#include "protorecord/Constants.h"

namespace protorecord
{
	class Reader
	{
	public:
		/**
		 * Constructor
		 *
		 * @param[in] filepath
		 * The absolute or relative filepath to the record.
		 */
		Reader(
			const std::string &filepath);

		/**
		 * Destructor
		 */
		~Reader();

		/**
		 * @return
		 * True if there is at least one more item that can be read
		 * from the record.
		 */
		bool
		has_next();

		/**
		 * Reads the next protobuf message from the record, but does not
		 * increment to the next item.
		 *
		 * @param[out] pb
		 * The google::protobuf message to read into
		 *
		 * @return
		 * True if message was successfully read, false otherwise
		 */
		template<class PROTOBUF_T>
		bool
		get_next(
			PROTOBUF_T &pb);

		/**
		 * Reads the next protobuf message from the record, and increments
		 * to the next item.
		 *
		 * @param[out] pb
		 * The google::protobuf message to read into
		 *
		 * @return
		 * True if message was successfully read, false otherwise
		 */
		template<class PROTOBUF_T>
		bool
		take_next(
			PROTOBUF_T &pb);

		/**
		 * Reads the next item's timestamp
		 *
		 * @param[out] item_timestamp
		 * The parsed item's timestamp
		 *
		 * @return
		 * True if record contains timestamps, and the item timestamp was
		 * read successfully.
		 */
		bool
		get_next_timestamp(
			uint64_t &item_timestamp);

		/**
		 * @return
		 * The number of items that can be read from the record
		 */
		size_t
		size();

		/**
		 * @return
		 * The record's bit mask of protorecord::Flags::* constants.
		 */
		uint32_t
		flags();

		/**
		 * @return
		 * True if the record's HAS_ASSUMED_DATA flag is set, false otherwise.
		 */
		bool
		has_assumed_data();

		/**
		 * @return
		 * True if the record contains timestamped items, false otherwise.
		 */
		bool
		has_timestamps();

		/**
		 * @param[out] start_time_us
		 * The records start time in microseconds
		 *
		 * @return
		 * True on success, false otherwise. If false is returned, the value
		 * of 'start_time_us' should not be assumed.
		 */
		bool
		get_start_time(
			uint64_t &start_time_us);

		/**
		 * @return
		 * The protorecord library version that the record was made with.
		 * This can be used to determine version incompatibilities. Returns
		 * version 0.0.0 if Reader was not initialized properly.
		 */
		protorecord::Version
		get_version();

		/**
		 * @return
		 * A string explaining the failure reason for a previously called
		 * method in this class. An empty string is returned if the previous
		 * method was successful. This method will also return an empty
		 * string upon subsequent calls, in affect "popping" the failure
		 * reason from the class.
		 */
		std::string
		reason();

	protected:
		/**
		 * Initializes the class for read. This method is called from
		 * the constructor and is used to setup the internal member
		 * variables.
		 *
		 * @param[in] filepath
		 * The record path to attempt to initial the class with
		 *
		 * @return
		 * True if successfully initialized, false otherwise
		 */
		bool
		init_record(
			const std::string &filepath);

		/**
		 * Closes all opened file descriptors. This method is automatically
		 * called by class's destructor.
		 */
		void
		close();

		/**
		 * @return
		 * True if record's version number is compatible with this version of
		 * Reader class.
		 */
		bool
		is_compatible(
			const Version &record_version);

		/**
		 * Parse the next index item from the index_file_ and places the
		 * result in the classes index_item_ member
		 *
		 * @param[in] item_idx
		 * The index item to read from the index_file
		 *
		 * @param[out] item_out
		 * The parsed IndexItem
		 *
		 * @return
		 * True if index_item was parsed successfully, false otherwise
		 */
		bool
		get_index_item(
			uint64_t item_idx,
			protorecord::IndexItem &item_out);

		/**
		 * @param[in] flag
		 * The flag to check for
		 *
		 * @return
		 * True if the record flags are valid and the flag is set
		 */
		bool
		is_flag_set(
			uint32_t flag);

	private:
		// set to true if the writer was initialized succesfully
		bool initialized_;

		// the parsed library version from the record
		protorecord::Version version_;

		// the parsed IndexSummary
		protorecord::IndexSummary index_summary_;

		// the current parsed index item
		protorecord::IndexItem index_item_;

		// the opened index file
		std::ifstream index_file_;

		// the opened data file
		std::ifstream data_file_;

		// buffer used to deserialize data from files
		std::vector<char> buffer_;

		// the next item index the class will read from
		uint64_t next_item_num_;

		// set to true if any internal failure occurs
		bool failbit_;

		// set to a human reasble string explaing previous method's failure
		std::string fail_reason_;

	};

	template<class PROTOBUF_T>
	bool
	Reader::get_next(
		PROTOBUF_T &pb)
	{
		bool okay = initialized_;
		fail_reason_ = "";

		okay = okay && has_next();
		okay = okay && get_index_item(next_item_num_,index_item_);

		if (okay)
		{
			// TODO could probably optimize this out
			data_file_.seekg(index_item_.offset());

			if (buffer_.size() < index_item_.size())
			{
				buffer_.resize(index_item_.size() * 2);
			}

			data_file_.read(buffer_.data(),index_item_.size());
			if (data_file_.eof())
			{
				fail_reason_ = "reached end of data file";
				okay = false;
			}
		}

		if (okay && ! pb.ParseFromArray((void*)buffer_.data(),index_item_.size()))
		{
			fail_reason_ = "protobuf parse failed";
			okay = false;
		}

		if ( ! okay)
		{
			failbit_ = true;
		}

		return okay;
	}

	template<class PROTOBUF_T>
	bool
	Reader::take_next(
		PROTOBUF_T &pb)
	{
		bool okay = get_next(pb);
		if (okay)
		{
			next_item_num_++;
		}
		return okay;
	}

}// protorecord
