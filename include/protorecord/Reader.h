#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "ProtorecordIndex.pb.h"
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
		has_next() const;

		/**
		 * Reads a protobuf message from the record
		 *
		 * @param[in] pb
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
		 * @return
		 * The number of items that can be read from the record
		 */
		size_t
		size() const;

		/**
		 * @return
		 * The record's bit mask of protorecord::Flags::* constants.
		 */
		uint32_t
		flags() const;

		/**
		 * @return
		 * True if the record contains timestamped items, false otherwise.
		 */
		bool
		has_timestamps() const;

		/**
		 * @return
		 * The protorecord library version that the record was made with.
		 * This can be used to determine version incompatibilities. Returns
		 * version 0.0.0 if Reader was not initialized properly.
		 */
		protorecord::Version
		get_version() const;

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
			const Version &record_version) const;

		/**
		 * Parse the next index item from the index_file_ and places the
		 * result in the classes index_item_ member
		 *
		 * @return
		 * True if index_item was parsed successfully, false otherwise
		 */
		bool
		parse_next_index_item();

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

	};

	template<class PROTOBUF_T>
	bool
	Reader::get_next(
		PROTOBUF_T &pb)
	{
		bool okay = initialized_;

		okay = okay && has_next();
		okay = okay && parse_next_index_item();

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
				std::cerr << __func__ << " - " <<
					"reached end of data file!" << std::endl;
				okay = false;
			}
		}

		if (okay)
		{
			try
			{
				pb.ParseFromArray((void*)buffer_.data(),buffer_.size());
			}
			catch (const std::exception &ex)
			{
				std::cerr << __func__ << " - caught std::exception " << std::endl;
				std::cerr << "what: " << ex.what() << std::endl;
				okay = false;
			}
			catch (...)
			{
				std::cerr << __func__ << " - caught unknown exception " << std::endl;
				okay = false;
			}
		}

		if (okay)
		{
			next_item_num_++;
		}
		else
		{
			failbit_ = true;
		}

		return okay;
	}
}// protorecord
