#include "protorecord/Utils.h"
#include "protorecord/Reader.h"

namespace protorecord
{
	//-------------------------------------------------------------------------
	// constructors/destructors
	//-------------------------------------------------------------------------

	Reader::Reader(
		const std::string &filepath)
	 : initialized_(false)
	 , index_summary_()
	 , index_item_()
	 , index_file_()
	 , data_file_()
	 , next_item_num_(0)
	 , failbit_(false)
	 , fail_reason_("")
	{
		buffer_.resize(64000);
		initialized_ = init_record(filepath);
	}

	Reader::~Reader()
	{
		close();
	}

	//-------------------------------------------------------------------------
	// public methods
	//-------------------------------------------------------------------------

	bool
	Reader::has_next()
	{
		fail_reason_ = "";
		return initialized_ &&
			! failbit_ &&
			next_item_num_ < index_summary_.total_items();
	}

	bool
	Reader::get_next_timestamp(
		uint64_t &item_timestamp)
	{
		fail_reason_ = "";
		bool okay = get_index_item(next_item_num_,index_item_);
		if (okay && index_item_.has_timestamp())
		{
			item_timestamp = index_item_.timestamp();
		}
		return okay;
	}

	size_t
	Reader::size()
	{
		fail_reason_ = "";
		if (initialized_)
		{
			return index_summary_.total_items();
		}
		return 0;
	}

	uint32_t
	Reader::flags()
	{
		if (initialized_)
		{
			fail_reason_ = "";
			return index_summary_.flags();
		}
		else
		{
			fail_reason_ = "Reader not initialized";
			return 0;
		}
	}

	bool
	Reader::has_assumed_data()
	{
		fail_reason_ = "";
		return is_flag_set(protorecord::Flags::HAS_ASSUMED_DATA);
	}

	bool
	Reader::has_timestamps()
	{
		fail_reason_ = "";
		return is_flag_set(protorecord::Flags::HAS_TIMESTAMPS);
	}

	bool
	Reader::get_start_time(
		uint64_t &start_time_us)
	{
		fail_reason_ = "";
		if ( ! initialized_)
		{
			fail_reason_ = "Reader not initialized";
		}
		start_time_us = index_summary_.start_time_utc();
		return initialized_;
	}

	protorecord::Version
	Reader::get_version()
	{
		fail_reason_ = "";
		if ( ! initialized_)
		{
			fail_reason_ = "Reader not initialized";
		}
		return version_;
	}

	//-------------------------------------------------------------------------
	// protected methods
	//-------------------------------------------------------------------------

	bool
	Reader::init_record(
			const std::string &filepath)
	{
		fail_reason_ = "";
		bool okay = true;

		// open the index file
		const auto INDEX_FLAGS = std::ofstream::in | std::ofstream::binary;
		const auto INDEX_FILEPATH = filepath + "/index";
		if (okay)
		{
			index_file_.open(INDEX_FILEPATH,INDEX_FLAGS);
			if ( ! index_file_.good())
			{
				fail_reason_ = "failed to open index file '" + INDEX_FILEPATH + "'";
				okay = false;
			}
		}

		// open the data file
		const auto DATA_FLAGS = std::ofstream::in | std::ofstream::binary;
		const auto DATA_FILEPATH = filepath + "/data";
		if (okay)
		{
			data_file_.open(DATA_FILEPATH,DATA_FLAGS);
			if ( ! data_file_.good())
			{
				fail_reason_ = "failed to open data file '" + DATA_FILEPATH + "'";
				okay = false;
			}
		}

		// set version to invalid default
		version_.set_major(0);
		version_.set_minor(0);
		version_.set_patch(0);

		// intialize index item
		index_item_.set_offset(0);
		index_item_.set_size(0);

		// initialize index summary
		index_summary_.set_total_items(0);
		index_summary_.set_start_time_utc(0);
		index_summary_.set_flags(0);

		try
		{
			// read library version from record
			if (okay)
			{
				uint8_t version_size = 0;
				index_file_.read((char*)&version_size,1);// TODO check return
				index_file_.read(buffer_.data(),version_size);
				if ( ! index_file_.eof())
				{
					version_.ParseFromArray(buffer_.data(),version_size);

					// check for compatibility
					if ( ! is_compatible(version_))
					{
						fail_reason_ = "file/library version incompatibility";
						okay = false;
					}
				}
				else
				{
					fail_reason_ = "index file too small to parse version";
					okay = false;
				}
			}

			// read IndexSummary from record
			if (okay)
			{
				uint8_t summary_size = 0;
				index_file_.seekg(SUMMARY_BLOCK_OFFSET);
				index_file_.read((char*)&summary_size,1);// TODO check return
				index_file_.read(buffer_.data(),summary_size);

				if ( ! index_file_.eof())
				{
					index_summary_.ParseFromArray(buffer_.data(),summary_size);
				}
				else
				{
					fail_reason_ = "index file too small to parse IndexSummary";
					okay = false;
				}
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
			fail_reason_ = "caught unknown exception";
			okay = false;
		}

		return okay;
	}

	void
	Reader::close()
	{
		index_file_.close();
		data_file_.close();
	}

	bool
	Reader::is_compatible(
		const Version &record_version)
	{
		// only one version right now
		return true;
	}

	bool
	Reader::get_index_item(
		uint64_t item_idx,
		protorecord::IndexItem &item_out)
	{
		fail_reason_ = "";
		bool okay = initialized_ && item_idx < this->size();

		if (okay)
		{
			// compute position to IndexItem in file
			std::streampos pos = ITEM_BLOCK_OFFSET + ITEM_BLOCK_STRIDE * item_idx;

			// seek to position and read
			uint8_t index_item_size = 0;
			index_file_.seekg(pos);
			index_file_.read((char*)&index_item_size,1);// TODO check return
			index_file_.read(buffer_.data(),index_item_size);
			if (index_file_.eof())
			{
				fail_reason_ = "reached end of index file";
				okay = false;
			}
			else
			{
				try
				{
					index_item_.ParseFromArray(buffer_.data(),index_item_size);
				}
				catch (const std::exception &ex)
				{
					fail_reason_ = "caught std::exception. what: ";
					fail_reason_ += ex.what();
					okay = false;
				}
				catch (...)
				{
					fail_reason_ = "caught unknown exception";
					okay = false;
				}
			}
		}

		return okay;
	}

	bool
	Reader::is_flag_set(
		uint32_t flag)
	{
		auto flags = this->flags();
		bool is_set = true;
		is_set = is_set && (flags & protorecord::Flags::VALID);
		is_set = is_set && (flags & flag);
		return is_set;
	}

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------

}// protorecord