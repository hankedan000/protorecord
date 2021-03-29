#pragma once

#include <chrono>
#include <string>
#include <fstream>
#include <vector>

#include "ProtorecordIndex.pb.h"
#include "protorecord/Constants.h"
#include "protorecord/Utils.h"

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

		/**
		 * Method that writes an externally serialized protobuf message
		 * to the record. This method assumes that the caller has
		 * properly serialized the protobuf message to the buffer. This
		 * method is provided for performance purposes because it skips
		 * the process of serialization and just write the binary data
		 * to the record.
		 *
		 * @param[in] msg_data
		 * Pointer to the serialized data buffer to write to disk
		 *
		 * @param[in] msg_data_size
		 * The size of the msg_data block in bytes
		 *
		 * @return
		 * True if the item was written successfully, false otherwise.
		 * If the record was successfully written, the HAS_ASSUMED_DATA
		 * flag will be set.
		 */
		bool
		write_assumed(
			const void *msg_data,
			uint32_t msg_data_size);

		/**
		 * @return
		 * The number of items that have been written thus far
		 */
		size_t
		size() const;

		/**
		 * Stores the finalized index to disk and closes all opened
		 * file descriptors. This method is automatically called by
		 * class's destructor.
		 *
		 * @param[in] store_readme
		 * Is true, a README.md will be stored inside the record
		 */
		void
		close(
			bool store_readme = true);

	protected:
		/**
		 * Initializes the recording for writing. This method is
		 * called from the constructor and is used to setup the
		 * internal member variables and the recording file structure
		 *
		 * @param[in] filepath
		 * The path to attempt to initial the recording in
		 *
		 * @param[in] allow_overwrite
		 * True if overwriting an existing record is allowed
		 *
		 * @return
		 * True if successfully initialized, false otherwise
		 */
		bool
		init_record(
			const std::string &filepath,
			bool allow_overwrite);

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

		/**
		 * Methods used to append the serialized item's data to the record
		 *
		 * @param[in] item_data
		 * Pointer to the serialized data buffer to write to disk
		 *
		 * @param[in] item_data_size
		 * The size of the item_data block in bytes
		 *
		 * @return
		 * True if the item was written successfully, if so the class's
		 * total_item_count_ is incremented.
		 */
		bool
		write_item_data(
			const void *item_data,
			uint32_t item_data_size);

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

		// the records filepath
		std::string record_path_;

		// the opened index file for this recording
		std::ofstream index_file_;

		// the opened data file where samples are recorded
		std::ofstream data_file_;

		// shared buffer used to serialize data to files
		std::vector<char> buffer_;

		// the total number of recorded samples thus far
		uint64_t total_item_count_;

		// the time when the recording was created
		std::chrono::microseconds start_time_;

		// bitmask of protorecord::Flags::*
		uint32_t flags_;

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
				// TODO should probably be using a monotonic clock source here
				index_item_.set_timestamp((get_time_now() - start_time_).count());
			}

			try
			{
				okay = okay && pb.SerializeToArray((void*)buffer_.data(),buffer_.size());
				okay = okay && write_item_data(buffer_.data(),pb.ByteSizeLong());
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

		return okay;
	}
}// protorecord
