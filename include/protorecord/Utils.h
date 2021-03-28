#pragma once

#include <chrono>

#include "ProtorecordTypes.pb.h"
#include "protorecord/version.h"

namespace protorecord
{
	/**
	 * @return
	 * library's major version
	 */
	inline
	unsigned int
	major_version()
	{
		return PROTORECORD_VERSION_MAJOR;
	}

	/**
	 * @return
	 * library's minor version
	 */
	inline
	unsigned int
	minor_version()
	{
		return PROTORECORD_VERSION_MINOR;
	}

	/**
	 * @return
	 * library's patch version
	 */
	inline
	unsigned int
	patch_version()
	{
		return PROTORECORD_VERSION_PATCH;
	}

	/**
	 * @return
	 * library's version as a protorecord::Version
	 */
	inline
	Version
	this_version()
	{
		Version v;
		v.set_major(major_version());
		v.set_minor(minor_version());
		v.set_patch(patch_version());
		return v;
	}

	/**
	 * @return
	 * the systemtime in microseconds
	 */
	inline
	std::chrono::microseconds
	get_time_now()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(
			std::chrono::system_clock::now().time_since_epoch());
	}

}// protorecord