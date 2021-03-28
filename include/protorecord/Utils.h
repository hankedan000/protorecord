#pragma once

#include "ProtorecordTypes.pb.h"
#include "protorecord/version.h"

namespace protorecord
{
	inline
	unsigned int
	major_version()
	{
		return PROTORECORD_VERSION_MAJOR;
	}

	inline
	unsigned int
	minor_version()
	{
		return PROTORECORD_VERSION_MINOR;
	}

	inline
	unsigned int
	patch_version()
	{
		return PROTORECORD_VERSION_PATCH;
	}

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

}// protorecord