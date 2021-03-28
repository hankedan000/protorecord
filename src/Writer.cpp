#include "protorecord/Utils.h"
#include "protorecord/Writer.h"
#include <iostream>
// TODO support non-unix systems
#include <sys/stat.h>

namespace protorecord
{
	//-------------------------------------------------------------------------
	// constructors/destructors
	//-------------------------------------------------------------------------

	Writer::Writer(
		const std::string &filepath)
	 : Writer(filepath,false)
	{
	}

	Writer::Writer(
		const std::string &filepath,
		bool enable_timestamping)
	 : initialized_(false)
	 , timestamping_enabled_(enable_timestamping)
	 , index_summary_()
	 , index_item_()
	 , index_file_()
	 , data_file_()
	 , total_item_count_(0)
	{
		buffer_.resize(64000);
		initialized_ = init_record(filepath);
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

	void
	Writer::close()
	{
		internal_close();
	}

	//-------------------------------------------------------------------------
	// protected methods
	//-------------------------------------------------------------------------

	bool
	Writer::init_record(
			const std::string &filepath)
	{
		bool okay = true;

		int status = mkdir(filepath.c_str(),0777);
		if (status < 0)
		{
			std::cerr << __func__ << " - failed to create record. " <<
				"error: " << strerror(errno) << "; "
				"filepath: '" << filepath << "'" << std::endl;
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
				std::cerr << "failed to create"
					" index file '" << INDEX_FILEPATH << "'" << std::endl;
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
				std::cerr << "failed to create"
					" data file '" << DATA_FILEPATH << "'" << std::endl;
				okay = false;
			}
		}

		// intialize index item
		uint32_t item_size = INDEX_ITEM_SIZE_NO_TIMESTAMP;
		index_item_.set_offset(0);
		index_item_.set_size(0);
		if (timestamping_enabled_)
		{
			item_size = INDEX_ITEM_SIZE_TIMESTAMP;
			index_item_.set_timestamp(0);
		}

		start_time_ = get_time_now();

		// initialize index summary
		index_summary_.set_total_items(total_item_count_);
		index_summary_.set_index_item_size(item_size);
		index_summary_.set_start_time_utc(start_time_.count());

		if (okay)
		{
			// store library version in index file first
			protorecord::Version version;
			version.set_major(protorecord::major_version());
			version.set_minor(protorecord::minor_version());
			version.set_patch(protorecord::patch_version());
			version.SerializeToArray((void*)buffer_.data(),buffer_.size());
			index_file_.write(buffer_.data(),VERSION_SIZE);

			// store current summary information in index file
			store_summary(VERSION_SIZE,false);// don't restore to previous position
		}

		return okay;
	}

	void
	Writer::internal_close()
	{
		if (initialized_)
		{
			store_summary(VERSION_SIZE,true);
			index_file_.close();
			data_file_.close();
		}
		initialized_ = false;
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

			// update total count
			index_summary_.set_total_items(total_item_count_);

			// save latest index summary
			index_summary_.SerializeToArray((void*)buffer_.data(),buffer_.size());
			index_file_.write(buffer_.data(),INDEX_SUMMARY_SIZE);

			if (restore_pos)
			{
				// restore position back
				index_file_.seekp(prev_pos);
			}
		}

		return okay;
	}

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------

}// protorecord