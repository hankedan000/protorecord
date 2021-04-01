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
		 * Default Constructor
		 */
		Writer();

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
		 * Opens a record for writing
		 * 
		 * @param[in] filepath
		 * Path to save record to
		 * 
		 * @param[in] enable_timestamping
		 * Set to true if you want to store a UTC timestamp for each
		 * recorded sample. Storing timestamps will increase the
		 * size of the index file.
		 *
		 * @return
		 * True if the Writer was successfully initialized, false if the
		 * record creation failed, or if the Writer was already opened.
		 */
		bool
		open(
			const std::string &filepath,
			bool enable_timestamping = false);

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
		size();

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
		 * @param[in] timestamp
		 * The timestamp of the item. If timestamping is disabled for this writer
		 * instance, then this argument is ignored.
		 *
		 * @return
		 * True if the item was written successfully, if so the class's
		 * total_item_count_ is incremented.
		 */
		bool
		write_item_data(
			const void *item_data,
			uint32_t item_data_size,
			const std::chrono::microseconds &timestamp);

	private:
		// set to true if the writer was initialized succesfully
		bool initialized_;

		// set to true if timestamp recording is enabled
		// this must be specified at constructor time
		bool timestamping_enabled_;

		// the live summary
		protorecord::IndexSummary index_summary_;

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

		// the system clock time when the recording was opened
		std::chrono::microseconds start_time_system_;

		// the monotonic clock time when the recording was opened
		std::chrono::microseconds start_time_mono_;

		// bitmask of protorecord::Flags::*
		uint32_t flags_;

		// set to a human reasble string explaing previous method's failure
		std::string fail_reason_;

	};

	template<class PROTOBUF_T>
	bool
	Writer::write(
		const PROTOBUF_T &pb)
	{
		bool okay = true;
		fail_reason_ = "";

		if (initialized_)
		{
			std::chrono::microseconds timestamp;
			if (timestamping_enabled_)
			{
				timestamp = get_mono_time() - start_time_mono_;
			}

			try
			{
				if (pb.SerializeToArray((void*)buffer_.data(),buffer_.size()))
				{
					okay = okay && write_item_data(buffer_.data(),pb.ByteSizeLong(),timestamp);
				}
				else
				{
					fail_reason_ = "failed to serialize protobuf msg";
					okay = false;
				}
			}
			catch (const std::exception &ex)
			{
				fail_reason_ = "caught std::exception. what: ";
				fail_reason_ += ex.what();
				okay = false;
			}
			catch (...)
			{
				fail_reason_ = "caught unknown exception ";
				okay = false;
			}
		}
		else
		{
			fail_reason_ = "Writer not initialized";
			okay = false;
		}

		return okay;
	}
}// protorecord
