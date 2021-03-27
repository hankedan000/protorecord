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
	{
		initialized_ = init_record(filepath);
		buffer_.resize(64000);
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

	//-------------------------------------------------------------------------
	// protected methods
	//-------------------------------------------------------------------------

	bool
	Writer::init_record(
			const std::string &filepath)
	{
		bool okay = true;

		int status = mkdir(filepath.c_str(),0777);
		if (okay && status < 0)
		{
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
		index_item_.set_offset(0);
		index_item_.set_size(0);
		if (timestamping_enabled_)
		{
			index_item_.set_timestamp(0);
		}

		// initialize index summary
		index_summary_.set_total_items(0);
		index_summary_.set_index_item_size(index_item_.ByteSizeLong());
		index_summary_.set_start_time_utc(0);// TODO initialize this

		if (okay)
		{
			// store library version in index file first
			protorecord::Version version;
			version.set_major(protorecord::major_version());
			version.set_minor(protorecord::minor_version());
			version.set_patch(protorecord::patch_version());
			version.SerializeToArray((void*)buffer_.data(),buffer_.size());
			index_file_.write(buffer_.data(),version.ByteSizeLong());

			// store current summary information in index file
			summary_pos_ = index_file_.tellp();
			store_summary(false);// don't restore to previous position

			// store location to where the first IndexItem will be stored
			first_item_pos_ = index_file_.tellp();
		}

		return okay;
	}

	void
	Writer::close()
	{
		if (initialized_)
		{
			store_summary(true);
			index_file_.close();
			data_file_.close();
		}
	}

	void
	Writer::store_summary(
		bool restore_pos)
	{
		if (initialized_)
		{
			// cache current position and seek to where index summary is stored
			auto prev_pos = index_file_.tellp();
			index_file_.seekp(summary_pos_);

			// save latest index summary
			index_summary_.SerializeToArray((void*)buffer_.data(),buffer_.size());
			index_file_.write(buffer_.data(),index_summary_.ByteSizeLong());

			if (restore_pos)
			{
				// restore position back
				index_file_.seekp(prev_pos);
			}
		}
	}

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------

}// protorecord