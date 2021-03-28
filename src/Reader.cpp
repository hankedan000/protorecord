#include "protorecord/Utils.h"
#include "protorecord/Reader.h"
#include <iostream>

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
	Reader::has_next() const
	{
		return initialized_ &&
			! failbit_ &&
			next_item_num_ < index_summary_.total_items();
	}

	bool
	Reader::get_next_timestamp(
		uint64_t &item_timestamp)
	{
		bool okay = get_index_item(next_item_num_,index_item_);
		if (okay && index_item_.has_timestamp())
		{
			item_timestamp = index_item_.timestamp();
		}
		return okay;
	}

	size_t
	Reader::size() const
	{
		if (initialized_)
		{
			return index_summary_.total_items();
		}
		return 0;
	}

	uint32_t
	Reader::flags() const
	{
		if (initialized_)
		{
			return index_summary_.flags();
		}
		return 0;
	}

	bool
	Reader::has_timestamps() const
	{
		auto flags = this->flags();
		bool has_timestamps = true;
		has_timestamps = has_timestamps && (flags & protorecord::Flags::VALID);
		has_timestamps = has_timestamps && (flags & protorecord::Flags::HAS_TIMESTAMPS);
		return has_timestamps;
	}

	bool
	Reader::get_start_time(
		uint64_t &start_time_us) const
	{
		start_time_us = index_summary_.start_time_utc();
		return initialized_;
	}

	protorecord::Version
	Reader::get_version() const
	{
		return version_;
	}

	//-------------------------------------------------------------------------
	// protected methods
	//-------------------------------------------------------------------------

	bool
	Reader::init_record(
			const std::string &filepath)
	{
		bool okay = true;

		// open the index file
		const auto INDEX_FLAGS = std::ofstream::in | std::ofstream::binary;
		const auto INDEX_FILEPATH = filepath + "/index";
		if (okay)
		{
			index_file_.open(INDEX_FILEPATH,INDEX_FLAGS);
			if ( ! index_file_.good())
			{
				std::cerr << "failed to open"
					" index file '" << INDEX_FILEPATH << "'" << std::endl;
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
				std::cerr << "failed to open"
					" data file '" << DATA_FILEPATH << "'" << std::endl;
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
		index_summary_.set_index_item_size(index_item_.ByteSizeLong());
		index_summary_.set_start_time_utc(0);// TODO initialize this

		try
		{
			// read library version from record
			if (okay)
			{
				index_file_.read(buffer_.data(),PROTORECORD_VERSION_SIZE);
				if ( ! index_file_.eof())
				{
					version_.ParseFromArray(buffer_.data(),buffer_.size());

					// check for compatibility
					if ( ! is_compatible(version_))
					{
						std::cerr << __func__ << " - " <<
							"detected version incompatibility!\n" <<
							" record version: " << version_.DebugString() << "; \n"
							" library version: " << get_version().DebugString() << "; \n";
						okay = false;
					}
				}
				else
				{
					std::cerr << __func__ << " - " <<
						"index file too small to parse version!" << std::endl;
					okay = false;
				}
			}

			// read IndexSummary from record
			if (okay)
			{
				index_file_.read(buffer_.data(),PROTORECORD_INDEX_SUMMARY_SIZE);

				if ( ! index_file_.eof())
				{
					index_summary_.ParseFromArray(buffer_.data(),buffer_.size());
				}
				else
				{
					std::cerr << __func__ << " - " <<
						"index file too small to parse IndexSummary!" << std::endl;
					okay = false;
				}
			}
		}
		catch (const std::exception &ex)
		{
			std::cerr << __func__ << " - " <<
				"caught std::exception" << std::endl;
			std::cerr << "what: " << ex.what();
			okay = false;
		}
		catch (...)
		{
			std::cerr << __func__ << " - " <<
				"caught unknown exception" << std::endl;
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
		const Version &record_version) const
	{
		// only one version right now
		return true;
	}

	bool
	Reader::get_index_item(
		uint64_t item_idx,
		protorecord::IndexItem &item_out)
	{
		bool okay = initialized_ && item_idx < this->size();

		if (okay)
		{
			// compute position to IndexItem in file
			std::streampos pos = PROTORECORD_VERSION_SIZE + PROTORECORD_INDEX_SUMMARY_SIZE;
			pos += item_idx * index_summary_.index_item_size();

			// seek to position and read
			index_file_.seekg(pos);
			index_file_.read(buffer_.data(),index_summary_.index_item_size());
			if (index_file_.eof())
			{
				std::cerr << __func__ << " - " <<
					"reached end of index file!" << std::endl;
				okay = false;
			}
			else
			{
				try
				{
					index_item_.ParseFromArray(buffer_.data(),buffer_.size());
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
		}

		return okay;
	}

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------

}// protorecord