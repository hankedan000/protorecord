#pragma once

#include <string>
#include <fstream>
#include <vector>

#include "ProtorecordIndex.pb.h"
#include "protorecord/Sizes.h"

namespace protorecord
{
	class Writer
	{
	public:
		/**
		 * Constructor
		 *
		 * @param[in] filepath
		 * The absolute or relative filepath to store the record.
		 */
		Writer(
			const std::string &filepath);

		/**
		 * Constructor
		 *
		 * @param[in] filepath
		 * The absolute or relative filepath to store the record.
		 *
		 * @param[in] enable_timestamping
		 * Set to true if you want to store a UTC timestamp for each
		 * recorded sample. Storing timestamps will increase the
		 * size of the index file.
		 */
		Writer(
			const std::string &filepath,
			bool enable_timestamping);

		/**
		 * Destructor
		 */
		~Writer();

		/**
		 * Write a protobuf message to the record
		 *
		 * @param[in] pb
		 * The google::protobuf message to write
		 *
		 * @return
		 * True if message was successfully written, false otherwise
		 */
		template<class PROTOBUF_T>
		bool
		write(
			const PROTOBUF_T &pb);

	protected:
		/**
		 * Initializes the recording for writing. This method is
		 * called from the constructor and is used to setup the
		 * internal member variables and the recording file structure
		 *
		 * @param[in] filepath
		 * The path to attempt to initial the recording in
		 *
		 * @return
		 * True if successfully initialized, false otherwise
		 */
		bool
		init_record(
			const std::string &filepath);

		/**
		 * Stores the finalized index to disk and closes all opened
		 * file descriptors. This method is automatically called by
		 * class's destructor.
		 */
		void
		close();

		/**
		 * Will store the current IndexSummary to disk
		 *
		 * @param[in] pos
		 * The location in the index file to store the summary
		 *
		 * @param[in] restore_pos
		 * If set to true, the index_file_ position will be restored
		 * back to where it was when this method was called.
		 *
		 * @return
		 * True if the summary was stored succesfully, false otherwise
		 */
		bool
		store_summary(
			std::streampos pos,
			bool restore_pos);

	private:
		// set to true if the writer was initialized succesfully
		bool initialized_;

		// set to true if timestamp recording is enabled
		// this must be specified at constructor time
		bool timestamping_enabled_;

		// the live summary
		protorecord::IndexSummary index_summary_;

		// the current index item
		protorecord::IndexItem index_item_;

		// the opened index file for this recording
		std::ofstream index_file_;

		// the opened data file where samples are recorded
		std::ofstream data_file_;

		// shared buffer used to serialize data to files
		std::vector<char> buffer_;

		// the total number of recorded samples thus far
		uint64_t total_item_count_;

	};

	template<class PROTOBUF_T>
	bool
	Writer::write(
		const PROTOBUF_T &pb)
	{
		bool okay = initialized_;

		if (okay)
		{
			if (timestamping_enabled_)
			{
				// TODO implement timestamping
				index_item_.set_timestamp(0);
			}

			index_item_.set_offset(data_file_.tellp());
			index_item_.set_size(pb.ByteSizeLong());
			if (index_item_.size() < buffer_.size())
			{// grow buffer
				buffer_.resize(index_item_.size() * 2);
			}

			try
			{
				pb.SerializeToArray((void*)buffer_.data(),buffer_.size());
				data_file_.write(buffer_.data(),index_item_.size());
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

			// if serialization was successful, update index file
			if (okay)
			{
				// store the IndexItem to index file
				index_item_.SerializeToArray((void*)buffer_.data(),buffer_.size());
				index_file_.write(buffer_.data(),INDEX_ITEM_SIZE);

				// increment item count
				total_item_count_++;
			}
		}

		return okay;
	}
}// protorecord
