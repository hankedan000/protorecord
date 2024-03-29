#include "protorecord/Constants.h"
#include "protorecord/Writer.h"
#include "Protorecord.pb.h"
// TODO support non-unix systems
#include <sys/stat.h>
#include <stdint.h>

namespace protorecord
{
	//-------------------------------------------------------------------------
	// constructors/destructors
	//-------------------------------------------------------------------------

	Writer::Writer()
	 : Writer("")
	{
	}

	Writer::Writer(
		const std::string &filepath)
	 : Writer(filepath,false)
	{
	}

	Writer::Writer(
		const std::string &filepath,
		bool enable_timestamping)
	 : initialized_(false)
	 , timestamping_enabled_()
	 , record_path_()
	 , index_file_()
	 , data_file_()
	 , total_item_count_(0)
	 , flags_(protorecord::Flags::VALID)
	 , fail_reason_("")
	{
		buffer_.resize(64000);
		open(filepath,enable_timestamping);
	}

	/**
	 * Destructor
	 */
	Writer::~Writer()
	{
		close();
	}

	//-------------------------------------------------------------------------
	// public methods
	//-------------------------------------------------------------------------

	bool
	Writer::open(
		const std::string &filepath,
		bool enable_timestamping)
	{
		fail_reason_ = "";

		if (initialized_)
		{
			// don't reinitialize file
			fail_reason_ = "Writer already intialized";
			return false;
		}

		if (filepath != "")
		{
			// reset member variables
			timestamping_enabled_ = enable_timestamping;
			record_path_ = filepath;
			total_item_count_ = 0;
			flags_ = protorecord::Flags::VALID;

			initialized_ = init_record(filepath,true);
		}

		return initialized_;
	}

	bool
	Writer::write_assumed(
		const void *msg_data,
		uint32_t msg_data_size)
	{
		bool okay = initialized_;
		fail_reason_ = "";

		std::chrono::microseconds timestamp;
		if (timestamping_enabled_)
		{
			timestamp = get_mono_time() - start_time_mono_;
		}

		okay = okay && write_item_data(msg_data,msg_data_size,timestamp);

		if (okay)
		{
			flags_ |= protorecord::Flags::HAS_ASSUMED_DATA;
		}

		return okay;
	}

	size_t
	Writer::size()
	{
		fail_reason_ = "";
		return total_item_count_;
	}

	void
	Writer::close(
		bool store_readme)
	{
		fail_reason_ = "";

		if (initialized_)
		{
			store_summary(SUMMARY_BLOCK_OFFSET,true);
			index_file_.close();
			data_file_.close();

			if (store_readme)
			{
				const auto README_FILEPATH = record_path_ + "/README.md";
				std::ofstream readme(README_FILEPATH);
				if (readme.good())
				{
					const std::string REPO_URL("https://github.com/hankedan000/protorecord");
					readme << "**THIS FILE IS AUTO GENERATED AND ITS FORMAT SHOULD NOT BE ASSUMED**" << std::endl;
					readme << "This directory was created with the [protorecord](" << REPO_URL << ") library." << std::endl;
					readme << "protorecord version: " << version_to_string(this_version()) << std::endl;
					time_t rawtime = start_time_system_.count() / 1000000.0;
					struct tm *timeinfo = localtime(&rawtime);
					char buffer[1024];
					strftime(buffer,sizeof(buffer),"%A %B %d, %G %r",timeinfo);
					readme << "Creation Time: " << buffer << std::endl;
					readme << "Items: " << total_item_count_ << std::endl;

					readme.close();
				}
			}
		}
		initialized_ = false;
	}

	std::string
	Writer::reason()
	{
		return std::move(fail_reason_);
	}

	//-------------------------------------------------------------------------
	// protected methods
	//-------------------------------------------------------------------------

	bool
	Writer::init_record(
			const std::string &filepath,
			bool allow_overwrite)
	{
		bool okay = true;

		int status = mkdir(filepath.c_str(),0777);
		if (status < 0 && allow_overwrite && errno == EEXIST)
		{
			// allow overwrite
		}
		else if (status < 0)
		{
			fail_reason_ = std::string(" - failed to create record. ") +
				"error: " + strerror(errno) + "; " +
				"filepath: '" + filepath + "'";
			okay = false;
		}

		// open the index file
		const auto INDEX_FLAGS = std::ofstream::out | std::ofstream::binary;
		const auto INDEX_FILEPATH = filepath + "/index";
		if (okay)
		{
			index_file_.open(INDEX_FILEPATH,INDEX_FLAGS);
			if ( ! index_file_.good())
			{
				fail_reason_ = "failed to create index file: " + INDEX_FILEPATH;
				okay = false;
			}
		}

		// open the data file
		const auto DATA_FLAGS = std::ofstream::out | std::ofstream::binary;
		const auto DATA_FILEPATH = filepath + "/data";
		if (okay)
		{
			data_file_.open(DATA_FILEPATH,DATA_FLAGS);
			if ( ! data_file_.good())
			{
				fail_reason_ = "failed to create data file: " + DATA_FILEPATH;
				okay = false;
			}
		}

		if (timestamping_enabled_)
		{
			flags_ |= protorecord::Flags::HAS_TIMESTAMPS;
		}

		start_time_system_ = get_system_time();
		start_time_mono_ = get_mono_time();

		if (okay)
		{
			// store library version in index file first
			protorecord::Version version;
			version.set_major(protorecord::major_version());
			version.set_minor(protorecord::minor_version());
			version.set_patch(protorecord::patch_version());
			uint8_t version_size = version.ByteSizeLong();
			version.SerializeToArray((void*)buffer_.data(),buffer_.size());
			index_file_.write((const char *)&version_size,1);
			index_file_.write(buffer_.data(),version_size);

			// store current summary information in index file
			store_summary(SUMMARY_BLOCK_OFFSET,false);// don't restore to previous position
		}

		return okay;
	}

	bool
	Writer::store_summary(
		std::streampos pos,
		bool restore_pos)
	{
		bool okay = index_file_.good();

		if (okay)
		{
			// cache current position and seek to where index summary is stored
			auto prev_pos = index_file_.tellp();
			index_file_.seekp(pos);

			// create index summary based on member variables
			static protorecord::IndexSummary summary;
			summary.set_total_items(total_item_count_);
			summary.set_start_time_utc(start_time_system_.count());
			summary.set_flags(flags_);

			// save latest index summary
			okay = okay && summary.SerializeToArray((void*)buffer_.data(),buffer_.size());
			uint8_t summary_size = summary.ByteSizeLong();
			index_file_.write((const char *)&summary_size,1);
			index_file_.write(buffer_.data(),summary_size);

			if (restore_pos)
			{
				// restore position back
				index_file_.seekp(prev_pos);
			}
		}

		return okay;
	}

	bool
	Writer::write_item_data(
		const void *item_data,
		uint32_t item_data_size,
		const std::chrono::microseconds &timestamp)
	{
		bool okay = true;

		if (initialized_)
		{
			// build an index item for this entry
			static protorecord::IndexItem index_item;
			index_item.set_file(0);// TODO use more files
			index_item.set_offset(data_file_.tellp());
			index_item.set_size(item_data_size);
			if (timestamping_enabled_)
			{
				index_item.set_timestamp(timestamp.count());
			}

			data_file_.write((const char *)item_data,item_data_size);

			// update index file
			if (index_item.SerializeToArray((void*)buffer_.data(),buffer_.size()))
			{
				uint32_t pos = ITEM_BLOCK_OFFSET + total_item_count_ * ITEM_BLOCK_STRIDE;
				uint8_t index_item_serialize_size = index_item.ByteSizeLong();
				index_file_.seekp(pos);
				index_file_.write((const char *)&index_item_serialize_size,1);
				index_file_.write(buffer_.data(),index_item_serialize_size);
				// increment item count
				total_item_count_++;
			}
			else
			{
				fail_reason_ = "**internal error** failed to serialize index_item_";
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

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------

}// protorecord